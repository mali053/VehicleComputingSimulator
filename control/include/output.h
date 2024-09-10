#ifndef __OUTPUT__
#define __OUTPUT__

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


// Singleton class which handles the export to BSON
class Output {
private:
    // Private constructor
    Output(std::string pathToFileSave, std::map<int, std::string> sensors);
    // Single instance of the class
    static std::unique_ptr<Output> instance;

    // Variables contains the BSON document
    bson_t *document;
    bson_t conditions;
    bson_t *currentCond;
    // Helper variable
    int counter;
    // Name of the file to which the information will be saved
    std::string fileName;

public:
    // Gets the singleton instance
    static Output &getInstance();

    // Initialize a new condition and put a given condition string in it
    void addNewCondition(std::string condition);
    // Add an array of actions to the last condition in the BSON document
    void addActionsToLastCondition(std::vector<std::pair<int, std::string>> actions);
    // Saves the BSON document to a file
    void saveToFile();
};

#endif // __OUTPUT__