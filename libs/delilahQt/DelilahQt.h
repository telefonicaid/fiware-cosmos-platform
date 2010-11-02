/*
 * DelilahQt.h
 *
 *  Created on: Oct 20, 2010
 *      Author: ania
 */

#ifndef DELILAHQT_H_
#define DELILAHQT_H_

class DelilahQtApp : public QApplication
{
public:
	DelilahQtApp(int& argc, char** argv)
		: QApplication(argc, argv) {};
	~DelilahQtApp() {};

public:
	MainWindow
};

#endif /* DELILAHQT_H_ */
