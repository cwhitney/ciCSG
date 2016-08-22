#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"
#include "cinder/CameraUi.h"

#include "ciCSG.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicSampleApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

	void updateMesh();

	//ofMesh m0, m1, mesh, otherMesh;

	ci::TriMeshRef m0, m1, mesh, otherMesh;

	//ciCSG::Polygon cp0, cp1;
	vector<vec3> intersectionPoints;

	ci::CameraPersp	mCam;
	CameraUi	mCamUi;

	ci::gl::BatchRef	b0, b1, resultBatch;
	int intersectType = 0;

	bool bShowResult = false;

	ci::gl::GlslProgRef		mShader;
};

void BasicSampleApp::setup()
{
	mCam.setPerspective(45.0f, getWindowAspectRatio(), 0.1, 10000);
	mCamUi = CameraUi(&mCam);
	mCamUi.connect(getWindow());
	mCamUi.enable();

	mShader = gl::getStockShader(gl::ShaderDef().color());

	// Use one cinder native object, and load one model to mesh together
	m0 = TriMesh::create( geom::Teapot().subdivisions(16) >> geom::Scale(10.0) );

	ObjLoader loader1(loadAsset("box1.obj"));
	m1 = TriMesh::create(loader1);

	if (!loader1.getAvailableAttribs().count(geom::NORMAL)) {
		m1->recalculateNormals();
	}

	b0 = gl::Batch::create(*m0 , mShader);
	b1 = gl::Batch::create(*m1, mShader);

	mesh = TriMesh::create();
	//updateMesh();

	gl::enableDepth();
}

void BasicSampleApp::keyDown(KeyEvent event )
{
	if (event.getChar() == ' ') {
		bShowResult = !bShowResult;
		//updateMesh();
	}else if (event.getChar() == '1') {
		intersectType = 0;
		updateMesh();
	}else if (event.getChar() == '2') {
		intersectType = 1;
		updateMesh();
	}else if (event.getChar() == '3') {
		intersectType = 2;
		updateMesh();
	}
}

void BasicSampleApp::updateMesh()
{
	if (intersectType == 0) { ciCSG::meshUnion(m0, m1, mesh); }
	else if (intersectType == 1) { ciCSG::meshDifference(m0, m1, mesh); }
	else if (intersectType == 2) { ciCSG::meshIntersection(m0, m1, mesh); }

	resultBatch = gl::Batch::create(*mesh, mShader);
}

void BasicSampleApp::update()
{
	// Uncomment to move mesh around each frame
	/*
	glm::mat4 transform;
	transform = glm::translate(transform, vec3(sin(getElapsedSeconds()) * 10, 0, 0));
	transform = glm::rotate(transform, 0.1f, vec3(0.0, 0.1f, 1.0f));

	vec3 *vertices = m1->getPositions<3>();
	size_t numVert = m1->getNumVertices();

	for (int i=0; i<numVert; ++i)
	{
		vec4 v4(*vertices, 1.0);
		vec3 newPos = vec3(v4 * transform);
		*vertices = newPos;
		vertices++;
	}

	b1 = gl::Batch::create(*m1, mShader);
	updateMesh();
	//*/
}

void BasicSampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableDepth();

	gl::ScopedMatrices scMat;
	gl::setMatrices(mCam);

	if(!bShowResult){
		gl::disableWireframe();

		gl::color(1, 0, 1);
		b0->draw();

		gl::color(0, 1, 1);
		b1->draw();

		gl::enableWireframe();

		gl::color(0, 0, 0);
		b0->draw();
		b1->draw();
	}

	if(bShowResult && resultBatch){
		gl::disableWireframe();
		gl::color(1, 0, 1);
		resultBatch->draw();

		gl::enableWireframe();
		gl::color(0, 0, 0);
		resultBatch->draw();
	}

	gl::drawString("Space to toggle between mesh and boolean operations", vec2(10,10) );
}

CINDER_APP(BasicSampleApp, RendererGl, [&](App::Settings *settings) {
	settings->setWindowSize(1200, 768);
})
