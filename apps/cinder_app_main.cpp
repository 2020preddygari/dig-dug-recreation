#include "visualizer/dig_dug_app.h"

using dig_dug::DigDugApp;

void prepareSettings(DigDugApp::Settings* settings) {
  settings->setResizable(false);
}

CINDER_APP(DigDugApp, ci::app::RendererGl, prepareSettings);