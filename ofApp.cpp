#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(25);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofEnableDepthTest();

	this->cap.open("D:\\MP4\\Pexels Videos 2697636.mp4");
	this->cap_size = cv::Size(1080, 1080);

	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->frame = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	cv::Mat src;
	this->cap >> src;
	if (src.empty()) {

		this->cap.set(cv::CAP_PROP_POS_FRAMES, 0);
		return;
	}

	cv::cvtColor(src, src, cv::COLOR_BGR2RGB);
	cv::Mat(src, cv::Rect(this->cap.get(cv::CAP_PROP_FRAME_WIDTH) * 0.5 - this->cap_size.width * 0.5, this->cap.get(cv::CAP_PROP_FRAME_HEIGHT) * 0.5 - this->cap_size.height * 0.5, this->cap_size.width, this->cap_size.height)).copyTo(this->frame);
	this->image.update();

	this->mesh_list.clear();
	this->frame_list.clear();

	float radius = 250;
	int deg_span = 1;
	int y_span = 27;
	for (int y = 0; y < this->cap_size.height; y += y_span) {

		radius -= 3;

		ofMesh mesh, frame;
		frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
		for (int deg = 0; deg < 360; deg += deg_span) {

			int index = mesh.getNumVertices();

			vector<glm::vec3> vertices;
			vertices.push_back(glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), y_span * -0.5));
			vertices.push_back(glm::vec3(radius * cos((deg + deg_span) * DEG_TO_RAD), radius * sin((deg + deg_span) * DEG_TO_RAD), y_span * -0.5));
			vertices.push_back(glm::vec3(radius * cos((deg + deg_span) * DEG_TO_RAD), radius * sin((deg + deg_span) * DEG_TO_RAD), y_span * 0.5));
			vertices.push_back(glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), y_span * 0.5));

			mesh.addVertices(vertices);
			frame.addVertices(vertices);

			int x = ofMap(deg, 0, 360, 0, this->cap_size.width);
			int next_x = ofMap(deg + deg_span, 0, 360, 0, this->cap_size.width);

			mesh.addTexCoord(glm::vec3(x, y, 0));
			mesh.addTexCoord(glm::vec3(next_x, y, 0));
			mesh.addTexCoord(glm::vec3(next_x, y + y_span, 0));
			mesh.addTexCoord(glm::vec3(x, y + y_span, 0));

			mesh.addIndex(index + 0); mesh.addIndex(index + 1); mesh.addIndex(index + 2);
			mesh.addIndex(index + 0); mesh.addIndex(index + 2); mesh.addIndex(index + 3);

			frame.addIndex(index + 0); frame.addIndex(index + 1);
			frame.addIndex(index + 2); frame.addIndex(index + 3);
		}

		this->mesh_list.push_back(mesh);
		this->frame_list.push_back(frame);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateX(90);
	ofRotateZ(245);

	for (int i = 0; i < this->mesh_list.size(); i++) {

		ofPushMatrix();

		ofRotateY(ofMap(ofNoise(ofRandom(1000),ofGetFrameNum() * 0.001), 0, 1, -360, 360));
		ofRotateX(ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.001), 0, 1, -360, 360));

		this->image.bind();
		ofSetColor(255);
		this->mesh_list[i].draw();
		this->image.unbind();

		ofSetColor(39);
		this->frame_list[i].drawWireframe();

		ofPopMatrix();
	}
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}