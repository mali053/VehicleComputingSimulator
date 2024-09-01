#ifndef OUTPUT
#define OUTPUT

#include <QJsonDocument>
#include <QJsonObject>
#include <fstream>
#include <iostream>
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <bson/bson.h>

#include <string>
#include <map>

using namespace std;


class Output
{
private:
    bson_t *document;
    bson_t conditions;
    bson_t *currentCond;
    int counter;
    string fileName;

public:
    Output(string pathToFileSave, map<int, string> sensors);
    void addNewCondition(string condition);
    void addActionsToLastCondition(map<int, string> actions);
    // Or move it into the d-tor
    void saveToFile();
};

#endif