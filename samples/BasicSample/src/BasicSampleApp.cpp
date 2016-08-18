#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"
#include "cinder/CameraUi.h"

//#include "ciCSG.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicSampleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	//ofMesh m0, m1, mesh, otherMesh;

	ci::TriMeshRef m0, m1, mesh, otherMesh;

	//ciCSG::Polygon cp0, cp1;
	vector<vec3> intersectionPoints;

	ci::CameraPersp	mCam;
	CameraUi	mCamUi;

	ci::gl::BatchRef	b0, b1;
};

void BasicSampleApp::setup()
{
	mCam.setPerspective(45.0f, getWindowAspectRatio(), 0.1, 10000);
	mCamUi = CameraUi(&mCam);
	mCamUi.connect(getWindow());
	mCamUi.enable();

	ObjLoader loader0( loadAsset("box0.obj") );
	m0 = TriMesh::create(loader0);

	ObjLoader loader1(loadAsset("box1.obj"));
	m1 = TriMesh::create(loader1);

	if (!loader0.getAvailableAttribs().count(geom::NORMAL)) {
		m0->recalculateNormals();
	}

	if (!loader1.getAvailableAttribs().count(geom::NORMAL)) {
		m1->recalculateNormals();
	}

	b0 = gl::Batch::create(*m0, gl::getStockShader(gl::ShaderDef().color()));
	b1 = gl::Batch::create(*m1, gl::getStockShader(gl::ShaderDef().color()));

	gl::enableDepth();
	//mBatch = gl::Batch::create(*mMesh, mGlsl);
}

void BasicSampleApp::mouseDown( MouseEvent event )
{
}

void BasicSampleApp::update()
{
	ci::TriMeshRef mesh0 = m0, mesh1 = m1;

	glm::mat4 transform;

	glm::translate(transform, vec3(sin(getElapsedSeconds()) * 10, 0, 0));
	//glm::rotate(transform, glm::vec3((float)getElapsedSeconds() * 10.0f, 0.1f, 1.0f));

	//transform.translate( sin(getElapsedSeconds()) * 10, 0, 0);
	//transform.rotate( getElapsedSeconds() * 10, 0, .1, 1);
	/*
	auto& vertices = mesh1.getVertices();
	for (auto& v : vertices)
	{
		v = v * transform;
	}

	ciCSG::meshUnion(mesh0, mesh1, mesh);
	*/
}

void BasicSampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	/*
	ofBackground(0, 0, 0);
	ofPushStyle();
	ofDisableDepthTest();

	ofSetLineWidth(1);

	camera.begin();

	ofPushMatrix();
	ofScale(10, 10, 10);

	ofSetColor(255, 45);
	mesh.draw();
	ofSetColor(255);
	mesh.drawWireframe();

	ofPopMatrix();

	camera.end();

	ofPopStyle();
	*/

	gl::ScopedMatrices scMat;

	gl::setMatrices(mCam);

	gl::color(1, 0, 1);
	b0->draw();

	gl::color(0, 1, 1);
	b1->draw();


}

CINDER_APP(BasicSampleApp, RendererGl, [&](App::Settings *settings) {
	settings->setWindowSize(1200, 768);
})
