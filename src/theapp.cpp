#include "theapp.h"

TheApp::TheApp(int &argc, char **argv, std::shared_ptr<Logger::Logger> ptr) : QApplication(argc,argv), m_logger_ptr(ptr){

}
