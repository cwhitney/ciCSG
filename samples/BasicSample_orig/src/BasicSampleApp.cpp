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

	//ofMesh m0, m1, mesh, otherMesh;

	ci::TriMeshRef m0, m1, mesh, otherMesh;

	//ciCSG::Polygon cp0, cp1;
	vector<vec3> intersectionPoints;

	ci::CameraPersp	mCam;
	CameraUi	mCamUi;

	ci::gl::BatchRef	b0, b1, resultBatch;
	int intersectType = 0;

	bool bShowResult = true;

	ci::gl::GlslProgRef		mPhong;
};

void BasicSampleApp::setup()
{
	mCam.setPerspective(45.0f, getWindowAspectRatio(), 0.1, 10000);
	mCamUi = CameraUi(&mCam);
	mCamUi.connect(getWindow());
	mCamUi.enable();

	//mPhong = gl::GlslProg::create(loadAsset("phong.vert"), loadAsset("phong.frag"));
	mPhong = gl::getStockShader(gl::ShaderDef().color());

	ObjLoader loader0( loadAsset("box0.obj") );
	m0 = TriMesh::create(loader0);

	ObjLoader loader1(loadAsset("box1.obj"));
	m1 = TriMesh::create(loader1);

	//if (!loader0.getAvailableAttribs().count(geom::NORMAL)) {
		m0->recalculateNormals();
	//}

	//if (!loader1.getAvailableAttribs().count(geom::NORMAL)) {
		m1->recalculateNormals();
	//}

	b0 = gl::Batch::create(*m0, mPhong);
	b1 = gl::Batch::create(*m1, mPhong);

	mesh = TriMesh::create();

	gl::enableDepth();
}

void BasicSampleApp::keyDown(KeyEvent event )
{

	if (event.getChar() == ' ') {
		bShowResult = !bShowResult;
	}else if (event.getChar() == '1') {
		intersectType = 0;
	}else if (event.getChar() == '2') {
		intersectType = 1;
	}else if (event.getChar() == '3') {
		intersectType = 2;
	}
}

void BasicSampleApp::update()
{
	//ci::TriMeshRef mesh0 = m0, mesh1 = m1;

	glm::mat4 transform;

	//transform = glm::translate(transform, vec3(sin(getElapsedSeconds()) * 10, 0, 0));
	//transform = glm::rotate(transform, 0.1f, vec3(0.0, 0.1f, 1.0f));

	//transform.translate( sin(getElapsedSeconds()) * 10, 0, 0);
	//transform.rotate( getElapsedSeconds() * 10, 0, .1, 1);

	vec3 *vertices = m1->getPositions<3>();
	size_t numVert = m1->getNumVertices();

	for (int i=0; i<numVert; ++i)
	{
		vec4 v4(*vertices, 1.0);
		vec3 newPos = vec3(v4 * transform);
		*vertices = newPos;
		vertices++;
	}

	b1 = gl::Batch::create(*m1, mPhong);
	

	if( intersectType == 0) ciCSG::meshUnion(m0, m1, mesh);
	else if (intersectType == 1) ciCSG::meshDifference(m0, m1, mesh);
	else if (intersectType == 2) ciCSG::meshIntersection(m0, m1, mesh);

	mesh->recalculateNormals();

	resultBatch = gl::Batch::create(*mesh, mPhong);
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

	if(bShowResult){
		gl::disableWireframe();
		gl::color(1, 0, 1);
		resultBatch->draw();

		//gl::ScopedDepth scD(false);

		gl::enableWireframe();
		gl::color(0, 0, 0);
		resultBatch->draw();
	}
}

CINDER_APP(BasicSampleApp, RendererGl, [&](App::Settings *settings) {
	settings->setWindowSize(1200, 768);
})
