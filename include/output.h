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
    // Private constructor
    Output(string pathToFileSave, map<int, string> sensors);
    // Single instance of the class
    static unique_ptr<Output> instance;

    bson_t *document;
    bson_t conditions;
    bson_t *currentCond;
    int counter;
    string fileName;


public:
    // Gets the singleton instance
    static Output &getInstance();

    void addNewCondition(string condition);
    void addActionsToLastCondition(map<int, string> actions);
    // Or move it into the d-tor
    void saveToFile();
};

#endif