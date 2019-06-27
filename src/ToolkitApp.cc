#include "ToolkitApp.h"
#include "render_util.h"
#include "errors.h"

#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <QKeySequence>
#include <QMessageBox>
#include <Qt3DCore>

ToolkitApp::ToolkitApp(QWidget *parent) {
	main_menu_bar = new QMenuBar(NULL);
	file_menu = main_menu_bar->addMenu("File");
	view_menu = main_menu_bar->addMenu("Views");

	file_menu->addAction("Load Model", this, "aaction_load_model()");
	file_menu->addAction("Reload Files", this, "aaction_reload_files()", QKeySequence::fromString("F5"));

	this->setMenuBar(main_menu_bar);

	main_display = new SceneWidget(this);
	this->setCentralWidget(main_display);

	main_display->addSceneObject(createGridFloor(-15., 15., 32));

	QDir::addSearchPath("", "meshes/");

	auto paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
	for (int i=0; i<paths.size(); i++) {
		QFileInfo check_file(paths.at(i));
		if (check_file.exists()) {
			QDir dir(paths.at(i));
			QDir::addSearchPath("", dir.path());
			if (dir.cd("meshes")) {
				QDir::addSearchPath("", dir.path());
			}
		}
	}

}

void ToolkitApp::action_reload_files() {
	for (auto model : loaded_models) {
		Qt3DCore::QEntity*  model_render_obj = model->getRenderObj();
		main_display->removeSceneObject(model_render_obj);
		model->reload();

		model_render_obj = model->getRenderObj();
		main_display->addSceneObject(model_render_obj);
	}
}

void ToolkitApp::action_load_model() {
	QFileDialog file_dialog (this, "Select Model File");

	file_dialog.setNameFilter(tr("MeshupModels (*lua)"));
	file_dialog.setFileMode(QFileDialog::ExistingFile);

	if (file_dialog.exec()) {
		loadModel (file_dialog.selectedFiles().at(0));
	}	
}


void ToolkitApp::loadModel(const QString &model_file) {
	RBDLModelWrapper *model = new RBDLModelWrapper();
	bool errors_happend = false;

	Qt3DCore::QEntity* model_scene_obj;

	try {
		model_scene_obj = model->loadFromFile(model_file);
	} catch (std::exception& e) {
		errors_happend = true;
		QMessageBox errorBox;
		errorBox.setText(e.what());
		errorBox.setStandardButtons(QMessageBox::Cancel);
		errorBox.setDefaultButton(QMessageBox::Cancel);
		errorBox.exec();
	} catch (const char* e) {
		errors_happend = true;
		QMessageBox errorBox;
		errorBox.setText(e);
		errorBox.setStandardButtons(QMessageBox::Cancel);
		errorBox.setDefaultButton(QMessageBox::Cancel);
		errorBox.exec();
	}

	if (!errors_happend) {
		loaded_models.push_back(model);
		main_display->addSceneObject(model_scene_obj);
	}

}

void ToolkitApp::addView(QString name, QWidget *view_widget, Qt::DockWidgetArea area) {
	QDockWidget* dock = new QDockWidget(name, this);
	dock->setWidget(view_widget);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	addDockWidget(area, dock);

	view_widgets.push_back(dock);
	view_menu->addAction(dock->toggleViewAction());
}
