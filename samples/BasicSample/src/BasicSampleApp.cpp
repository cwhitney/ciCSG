#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"
#include "cinder/CameraUi.h"

#include "cinder/params//Params.h"

#include "ciCSG.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicSampleApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

	void updateMesh();

	ci::CameraPersp		mCam;
	CameraUi			mCamUi;

	ci::TriMeshRef		m0, m1, mesh;
	ci::gl::BatchRef	b0, b1, resultBatch;

	ci::gl::GlslProgRef			mShader;
	ci::params::InterfaceGlRef	mGui;

	int	mGeomType = 0, mBooleanType = 0;
	bool bAnimate = false;
	bool bDrawWireframe = true;
};

void BasicSampleApp::setup()
{
	mGui = params::InterfaceGl::create("ciCSG", vec2(200, 400));
	vector<string> mGeomTypeList = { "Cube", "Sphere", "Teapot", "Torus", "TorusKnot", "Cone" };
	vector<string> mBooleanTypeList = { "Union", "Difference", "Intersection" };

	mGui->addParam("Geometry", mGeomTypeList, &mGeomType)
		.updateFn([this] { 
		if (mGeomType == 0) {
			m0 = TriMesh::create(geom::Cube() >> geom::Scale(10.0));
		}else if (mGeomType == 1) {
			m0 = TriMesh::create(geom::Sphere().subdivisions(8) >> geom::Scale(10.0));
		}else if (mGeomType == 2) {
			m0 = TriMesh::create(geom::Teapot().subdivisions(4) >> geom::Scale(10.0));
		}else if (mGeomType == 3) {
			m0 = TriMesh::create(geom::Torus() >> geom::Scale(8.0));
		}else if (mGeomType == 4) {
			m0 = TriMesh::create(geom::TorusKnot() >> geom::Scale(5.0));
		}else if (mGeomType == 5) {
			m0 = TriMesh::create(geom::Cone() >> geom::Scale(7.0));
		}

		b0 = gl::Batch::create(*m0, mShader);
		updateMesh();
	});

	mGui->addParam("Boolean Types", mBooleanTypeList, &mBooleanType).updateFn([this] {
		updateMesh();
	});

	mGui->addParam("Animate", &bAnimate);
	mGui->addParam("Draw Wireframe", &bDrawWireframe);


	mCam.setPerspective(45.0f, getWindowAspectRatio(), 0.1, 10000);
	mCamUi = CameraUi(&mCam);
	mCamUi.connect(getWindow());
	mCamUi.enable();

	mShader = gl::getStockShader(gl::ShaderDef().color());

	// Use one cinder native object, and load one model to mesh together
	m0 = TriMesh::create(geom::Cube() >> geom::Scale(10.0));

	ObjLoader loader1(loadAsset("box1.obj"));
	m1 = TriMesh::create(loader1);

	if (!loader1.getAvailableAttribs().count(geom::NORMAL)) {
		m1->recalculateNormals();
	}

	b0 = gl::Batch::create(*m0 , mShader);
	b1 = gl::Batch::create(*m1, mShader);

	mesh = TriMesh::create();

	gl::enableAlphaBlending();
	gl::enableDepth();
	
	updateMesh();
}

void BasicSampleApp::updateMesh()
{
	if (mBooleanType == 0) { ciCSG::meshUnion(m0, m1, mesh); }
	else if (mBooleanType == 1) { ciCSG::meshDifference(m0, m1, mesh); }
	else if (mBooleanType == 2) { ciCSG::meshIntersection(m0, m1, mesh); }

	resultBatch = gl::Batch::create(*mesh, mShader);
}

void BasicSampleApp::update()
{
	// Uncomment to move mesh around each frame
	
	if (bAnimate) {
		glm::mat4 transform;
		transform = glm::translate(transform, vec3(sin(getElapsedSeconds()) * 10, 0, 0));
		transform = glm::rotate(transform, 0.1f, vec3(0.0, 0.1f, 1.0f));

		vec3 *vertices = m1->getPositions<3>();
		size_t numVert = m1->getNumVertices();

		for (int i = 0; i < numVert; ++i)
		{
			vec4 v4(*vertices, 1.0);
			vec3 newPos = vec3(v4 * transform);
			*vertices = newPos;
			vertices++;
		}

		b1 = gl::Batch::create(*m1, mShader);
		updateMesh();
	}
	//*/
}

void BasicSampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	// draw the original
	{
		gl::ScopedMatrices scMat;

		mCam.setAspectRatio(getWindowWidth() / 2.0 / getWindowHeight());
		gl::ScopedViewport scV(vec2(getWindowWidth() / 2.0, getWindowHeight()));

		gl::setMatrices(mCam);

		gl::disableWireframe();
		gl::color(1, 0, 1);
		b0->draw();

		gl::color(0, 1, 1);
		b1->draw();

		if (bDrawWireframe) {
			gl::enableWireframe();
			gl::color(0, 0, 0);
			b0->draw();
			b1->draw();
		}
	}

	// draw the boolean operation
	{
		gl::ScopedMatrices scMat;

		mCam.setAspectRatio(getWindowWidth() / 2.0 / getWindowHeight());
		gl::ScopedViewport scV(vec2(getWindowWidth() / 2.0, 0), vec2(getWindowWidth() / 2.0, getWindowHeight()));

		gl::setMatrices(mCam);
		//gl::translate();

		gl::disableWireframe();
		gl::color(1, 0, 1);
		resultBatch->draw();

		if (bDrawWireframe) {
			gl::enableWireframe();
			gl::color(0, 0, 0);
			resultBatch->draw();
		}
	}

	gl::disableWireframe();
	mGui->draw();
}

CINDER_APP(BasicSampleApp, RendererGl, [&](App::Settings *settings) {
	settings->setWindowSize(1200, 768);
})
