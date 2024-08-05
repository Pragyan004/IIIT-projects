#include <iostream>
#include <fstream>
#include <regex>
#include <string>

using namespace std;

// Structure to hold analysis results
struct AnalysisResult {
    int totalLines;                // Total number of lines in the file
    int totalComments;             // Total number of comments in the file
    int totalForLoops;             // Total number of for loops
    int totalWhileLoops;           // Total number of while loops
    int totalDoWhileLoops;         // Total number of do-while loops
    int totalIfStatements;         // Total number of if statements
    int totalElseStatements;       // Total number of else statements
    int totalElseIfStatements;     // Total number of else-if statements
    int totalSwitchStatements;     // Total number of switch statements
    int totalFunctions;            // Total number of functions
    int cyclomaticComplexity;      // Cyclomatic complexity of the code
};

// Check if the character at a given position in the line is inside a comment or string literal
bool isCommentOrString(const string& line, size_t pos) {
    bool inString = false;        // Flag to track if we are inside a string literal
    bool inComment = false;       // Flag to track if we are inside a multi-line comment
    size_t i = 0;                 // Current position in the line

    // Iterate through the line up to the given position
    while (i < pos) {
        // Toggle inString flag when encountering an unescaped quote
        if (line[i] == '"' && (i == 0 || line[i - 1] != '\\')) {
            inString = !inString;
        }

        // Check for comments only if not inside a string literal
        if (!inString) {
            // Detect the start of a single-line comment
            if (line.substr(i, 2) == "//") {
                return true;
            }
            // Detect the start of a multi-line comment
            if (line.substr(i, 2) == "/*") {
                inComment = true;
            }
            // Detect the end of a multi-line comment
            if (line.substr(i, 2) == "*/") {
                inComment = false;
            }
        }
        i++;
    }

    // Return true if inside a comment or string literal
    return inComment || inString;
}

// Analyze the given file for code metrics
void analyzeFile(const string& filename, AnalysisResult& result) {
    ifstream file(filename); // Open the file
    if (!file.is_open()) {  // Check if the file was successfully opened
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    string line;            // Current line of the file
    // Regular expressions to match different constructs
    regex forLoopRegex(R"(\bfor\s*\()"), whileLoopRegex(R"(\bwhile\s*\()"), 
          doWhileLoopRegex(R"(\bdo\s*\{)"), ifStatementRegex(R"(\bif\s*\()"), 
          elseStatementRegex(R"(\belse\b)"), elseIfStatementRegex(R"(\belse\s+if\s*\()"), 
          switchStatementRegex(R"(\bswitch\s*\()"), functionRegex(R"(\b[a-zA-Z_][a-zA-Z0-9_]*\s+\b[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*\{)");

    bool inMultiLineComment = false; // Flag to track if we are inside a multi-line comment

    // Read the file line by line
    while (getline(file, line)) {
        result.totalLines++; // Increment the total line count

        // Handle multi-line comments
        if (inMultiLineComment) {
            result.totalComments++; // Increment comment count for multi-line comments
            // Check if multi-line comment ends
            if (line.find("*/") != string::npos) {
                inMultiLineComment = false;
            }
            continue;
        }

        // Check for the start of a multi-line comment
        if (line.find("/*") != string::npos) {
            inMultiLineComment = true;
            result.totalComments++;
            continue;
        }

        // Check for single-line comments
        if (regex_search(line, regex(R"(\/\/)"))) {
            result.totalComments++;
            continue;
        }

        // Analyze loops
        if (regex_search(line, forLoopRegex) && !isCommentOrString(line, line.find("for"))) {
            result.totalForLoops++;
            result.cyclomaticComplexity++;
        } else if (regex_search(line, whileLoopRegex) && !isCommentOrString(line, line.find("while"))) {
            result.totalWhileLoops++;
            result.cyclomaticComplexity++;
        } else if (regex_search(line, doWhileLoopRegex) && !isCommentOrString(line, line.find("do"))) {
            result.totalDoWhileLoops++;
            result.cyclomaticComplexity++;
        }

        // Analyze conditional statements
        if (regex_search(line, ifStatementRegex) && !isCommentOrString(line, line.find("if"))) {
            result.totalIfStatements++;
            result.cyclomaticComplexity++;
        } else if (regex_search(line, elseIfStatementRegex) && !isCommentOrString(line, line.find("else if"))) {
            result.totalElseIfStatements++;
            result.cyclomaticComplexity++;
        } else if (regex_search(line, elseStatementRegex) && !isCommentOrString(line, line.find("else"))) {
            result.totalElseStatements++;
            result.cyclomaticComplexity++;
        } else if (regex_search(line, switchStatementRegex) && !isCommentOrString(line, line.find("switch"))) {
            result.totalSwitchStatements++;
            result.cyclomaticComplexity++;
        }

        // Analyze functions
        if (regex_search(line, functionRegex) && !isCommentOrString(line, line.find('('))) {
            result.totalFunctions++;
            result.cyclomaticComplexity++;
        }
    }

    file.close(); // Close the file
}

// Print the analysis results
void printAnalysisResult(const AnalysisResult& result) {
    cout << "Total lines: " << result.totalLines << endl;
    cout << "Total comments: " << result.totalComments << endl;

    // Print loop statistics
    cout << "\nTotal loops: " << (result.totalForLoops + result.totalWhileLoops + result.totalDoWhileLoops) << endl;
    cout << "  For loops: " << result.totalForLoops << endl;
    cout << "  While loops: " << result.totalWhileLoops << endl;
    cout << "  Do-while loops: " << result.totalDoWhileLoops << endl;

    // Print conditional statement statistics
    cout << "\nTotal conditional statements: " << (result.totalIfStatements + result.totalElseStatements + result.totalElseIfStatements + result.totalSwitchStatements) << endl;
    cout << "  If statements: " << result.totalIfStatements << endl;
    cout << "  Else-if statements: " << result.totalElseIfStatements << endl;
    cout << "  Else statements: " << result.totalElseStatements << endl;
    cout << "  Switch statements: " << result.totalSwitchStatements << endl;

    // Print function statistics
    cout << "\nTotal functions: " << result.totalFunctions << endl;

    // Print cyclomatic complexity
    cout << "\nCyclomatic complexity: " << result.cyclomaticComplexity + 1 << endl;
}

// Check if the filename has a valid extension (.c or .cpp)
bool hasValidExtension(const string& filename) {
    size_t dotPos = filename.rfind('.'); // Find the last dot in the filename
    if (dotPos == string::npos) {        // If no dot is found
        return false;
    }

    string extension = filename.substr(dotPos); // Extract the extension
    return (extension == ".c" || extension == ".cpp"); // Check for valid extensions
}

int main() {
    string filename; // Variable to store the filename
    cout << "Enter the filename: ";
    cin >> filename; // Read the filename from user input

    // Check if the file has a valid extension
    if (!hasValidExtension(filename)) {
        cerr << "Error: Please enter a file with a .c or .cpp extension." << endl;
        return 1;
    }

    AnalysisResult result = {0}; // Initialize the result structure

    analyzeFile(filename, result); // Analyze the file

    printAnalysisResult(result);   // Print the analysis results

    return 0;
}
