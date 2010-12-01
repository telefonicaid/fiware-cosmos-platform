/*
 * FILE:		Operation.cpp
 *
 * AUTHOR:		ania
 *
 * DESCRIPTION:
 *
 */

#ifndef OPERATION_CPP_
#define OPERATION_CPP_

#include <QStringList>

#include "Operation.h"

void Operation::upload(ss::network::Operation* o)
{
	help = QString::fromStdString(o->help());
	help_line = QString::fromStdString(o->help_line());

	for(int i=0; i< o->input_size(); i++)
	{
		ss::network::KVFormat format = o->input(i);
		QString key = QString::fromStdString(format.keyformat());
		QString value = QString::fromStdString(format.valueformat());
		KVPair* kv_pair = new KVPair(key, value);
		input.append(kv_pair);
	}

	for(int i=0; i<o->output_size(); i++)
	{
		ss::network::KVFormat format = o->output(i);
		QString key = QString::fromStdString(format.keyformat());
		QString value = QString::fromStdString(format.valueformat());
		KVPair* kv_pair = new KVPair(key, value);
		output.append(kv_pair);
	}
}

QString Operation::getHTMLInfo()
{
	QList< QStringList > info;
	QStringList name_info = (QStringList() << "Name: " << name);

	QStringList input_info = (QStringList() << "Inputs: ");
	QString inputs;
	for(int i=0; i<input.size(); i++)
	{

		inputs.append(input[i]->getKey());
		inputs.append(" ");
		inputs.append(input[i]->getValue());
		if (i < input.size()-1)
			inputs.append("<br>");
	}
	input_info << inputs;

	QStringList output_info = (QStringList() << "Outputs: ");
	QString outputs;
	for(int i=0; i<output.size(); i++)
	{

		outputs.append(output[i]->getKey());
		outputs.append(" ");
		outputs.append(output[i]->getValue());
		if (i < output.size()-1)
			outputs.append("<br>");
	}
	output_info << outputs;
	info << name_info << input_info << output_info;

	QString text;
	text += "<table>";
	for(int i=0; i<info.size(); i++)
	{
		text += "<tr>";
		text += "<td><b>" + info[i][0] + "</b></td>";
		text += "<td>" + info[i][1] + "</td>";
		text += "</tr>";
	}
	text += "</table>";

	if (! help.isEmpty())
	{
		text += "<b>Description:</b><br>";
		text += help;
	}

	return text;
}

#endif /* OPERATION_CPP_ */
