/*
 * FILE:		BaseObject.h
 *
 * AUTHOR:		Anna Wojdel
 *
 * DESCRIPTION:
 *
 */

#ifndef OBJECT_H_
#define OBJECT_H_


class BaseObject
{
public:
	BaseObject(const QString &_name)
		: name(_name) {};
	~BaseObject() {};

	QString getName() { return name; };
	virtual QString getHTMLInfo() = 0;

protected:
	QString name;
};

#endif /* OBJECT_H_ */
