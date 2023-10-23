//*******    Yuan: Make file
//*******
//*******
//*******


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <array>
#include <vector>
#include <list>
#include<algorithm>

#define DB_MAX_SIZE 100


using namespace std;

// Read from library file and record each gate's cell_delay and output_slew table in myMap
map<string, array<array<array<double, 7>, 7>, 2> > myMap;

map<int, string> intToStringMap; // Recorder to find each gate in myMap
int counter = 1;

// Read from circuit file and initialize size to 1000
vector< list<int> > gates(1000);

int parseLib(char *fName);
int parseCircuit(char *fName);
int outputEach(char *nodeNumber);

struct ParenCommaEq_is_space : std::ctype<char> {
    ParenCommaEq_is_space() : std::ctype<char>(get_table()) {}
    static mask const* get_table()
    {
        static mask rc[table_size];
        rc['('] = std::ctype_base::space;
        rc[')'] = std::ctype_base::space;
        rc[','] = std::ctype_base::space;
        rc['='] = std::ctype_base::space;
        rc[' '] = std::ctype_base::space;
        rc['\t'] = std::ctype_base::space;
        return &rc[0];
        }
};

int main(int argc, char* argv[]) {
    if (argc == 1) {
        cout << "Please input the LIB file and CIRCUIT file\n";
        return -1;
    }

    if (argc == 2) {
        cout << "Please input the CIRCUIT file\n";
        return -1;
    }

    if (argc == 3) {
        cout << "Please input at least one node number\n";
        return -1;
    }
    //First step: Read LIB file and store cell_delay and output_slew Table for each gate in myMap
    
    parseLib(argv[1]);
    parseCircuit(argv[2]);
    intToStringMap[-1] = "INPUT";


    //Second step: Read circuit file for each node numbers
    for (int i = 3; i < argc; i++){
        if ((stoi(argv[i]) > gates.size()) || (gates[stoi(argv[i])-1].empty())) {// Give a message if a requested node isn’t in the .isc file
            cout << "The requested node isn't in the file" << endl;
        } else {
            outputEach(argv[i]);
        }
    }
    return 0;
}

//Output each input node number
int outputEach(char *nodeNumber){
    int node = stoi(nodeNumber); // number

    list<int> myLinkedList = gates[node-1];
    string gateType = intToStringMap[myLinkedList.front()];// gate type
    string gateTypeLower = gateType;
    //Output lowercase gate type
    std::transform(gateTypeLower.begin(), gateTypeLower.end(), gateTypeLower.begin(), ::tolower);

    bool isGate;
    if (gateType == "INPUT"){
        isGate = false;
    }else{
        isGate = true;
    }
    // If node is "INPUT" or "OUTPUT", we don't need to output the following gate info
    if (isGate){
        list<int>::iterator it = next(myLinkedList.begin(), 1); 
        int firstElement = *it;   //First input number

        int lastElement = myLinkedList.back(); //Last input number
        auto f = myMap.find(gateType);
        cout << to_string(node) << " " << gateTypeLower << " " << to_string(firstElement) << " " << to_string(lastElement) << " " ;

        if (f != myMap.end()) {
            cout << myMap[gateType][1][2][1] << endl;
        } else {
            cout << 0 << endl;  // if encounter a gate type that is not present in your library, assume all zero entries
        }

    }else{
        cout << to_string(node) << " " << gateTypeLower << endl;

    }

    return 0;
}


// Circuit file
int parseCircuit(char *fName){

    ifstream ifs(fName);
	if (!ifs.is_open()) {
        cout << "Error opening file " << fName << endl;
		return -1;
	}

    // Keep reading file while no stream error occurs
    // like end of file (eof) etc.

	while (ifs.good()) {
        
        string lineStr;
        getline(ifs, lineStr);
        istringstream ifs(lineStr);
        // containing one line of text from the file
        ifs.imbue(locale(cin.getloc(),new ParenCommaEq_is_space));
        string firstWord;
        ifs >> firstWord;


         // Check if the input info line
        if (firstWord.compare("INPUT") == 0) {
            string second_word;
            ifs >> second_word;
            // the second word will be a input node number
            // We need to initialize each input node's linked list in vector with initialize "-1", which means INPUT
            int gateID = stoi(second_word);

            if (gateID > gates.size()){ // Here means we need to resize the vector
                int thousandsPlace = gateID / 1000;
                if ((thousandsPlace * 1000) == gateID){ //Determine if gateID is already a multiple of 1000 or not
                    gates.resize(gateID, list<int>());
                }else{  // If not, plus 1000 more
                    gates.resize((thousandsPlace + 1) * 1000, list<int>());

                }
            }
            gates[gateID-1].push_back(-1);

        }else if(isdigit(firstWord[0])){ // determine whether the first index of is a digital number or not
            // If it is, here we find the line about gate ID
            int gateID = stoi(firstWord);
            // Same as above
            if (gateID > gates.size()){ // Here means we need to resize the vector
                int thousandsPlace = gateID / 1000;
                if ((thousandsPlace * 1000) == gateID){ //Determine if gateID is already a multiple of 1000 or not
                    gates.resize(gateID, list<int>());
                }else{  // If not, plus 1000 more
                    gates.resize((thousandsPlace + 1) * 1000, list<int>());

                }
            }
            

            // Firstly determine which type of gate
            string second_word;
            ifs >> second_word;
            transform(second_word.begin(), second_word.end(), second_word.begin(), ::toupper);

                // Iterate through the intToStringMap to find if there is a key that matches the value of the string second_word.
            bool isFind = false;
            int keyNumber;
            for (const auto& pair : intToStringMap) {
                if (pair.second == second_word) {
                    // Get key number
                    keyNumber = pair.first;
                    isFind = true;
                    break;
                }
            }
            // If not found, record new element
            if (!isFind) {
                intToStringMap[counter] = second_word;
                gates[gateID-1].push_back(counter); // Give linked list an indicator of gate type
                counter++;
            }else{
                gates[gateID-1].push_back(keyNumber); // Give linked list an indicator of gate type
            }
            
            // Secondly record each input node number in the linked list
            string third_word;
            ifs >> third_word;
            gates[gateID-1].push_back(stoi(third_word));

            //Determine if there is an input gate number after it
            while (ifs >> third_word && isdigit(third_word[0])) {
                // Convert extracted strings to integers
                long number;
                number = stoi(third_word);
                gates[gateID-1].push_back(number);
            }

        }
	}
    ifs.close();
    return 0;
};


// Library File
int parseLib(char *fName) {
    // ------------------------------------------------------------------
    // Part 1. Parsing Lib file
    // ------------------------------------------------------------------
    // Open file. Check if opened correctly
    // Always check for this to save a lot of pain if an error occurs
	ifstream ifs(fName);
	if (!ifs.is_open()) {
        cout << "Error opening file " << fName << endl;
		return -1;
	}

    size_t db_idx = 0;
    string key;

    // Keep reading file while no stream error occurs
    // like end of file (eof) etc.
	while (ifs.good()) {
        string first_word;
        ifs >> first_word;

        // An info entry must start with "cell" as the first word exactly.
        if (first_word.compare("cell") == 0) {

            string second_word;
            ifs >> second_word;

            // The gate name has the format of "(<gate_name>)".
            // So find the index till the closing bracket.
            // Make sure to ignore the first index which has the opening bracket (hence the 1 and -1)
            size_t delim_pos = second_word.find(")");
            string gate_name = second_word.substr(1, delim_pos-1);

            key = gate_name;

            // Create two 7x7 arrays, 0 for cell_delay, 1 for output_slew
             array<array<array<double, 7>, 7>, 2> newArray;
            for (int k = 0; k < 2; k++) {
                for (int i = 0; i < 7; i++) {
                    for (int j = 0; j < 7; j++) {
                        newArray[k][i][j] = 0.0; // default to set all value to 0.0
                    }
                }
            }
            //Inserting key-value pairs into a map
            myMap[key] = newArray;

        }
        // The cell delays will start after the word "cell_delay(Timing_7_7)" exactly
        else if (first_word.compare("cell_delay(Timing_7_7)") == 0) {
            // Read 3 lines that contain the rest of above match, index 1 and index 2
            string tmp;
            getline(ifs, tmp);
            getline(ifs, tmp);
            getline(ifs, tmp);

            // From here on the next 7 lines will contain our delays
            for (size_t i = 0; i < 7; i++) {
                getline(ifs, tmp);

                // The delays will be between " ". Find the opening ".
                size_t start_delim_idx = tmp.find("\"");

                // Find the closing ".
                // The second argument is where we want to start our search
                // Ignore the first match so we don't get the same index again
                size_t end_delim_idx = tmp.find("\"", start_delim_idx + 1);

                // The second arg in substr in no. of characters, not the ending index
                string data_str = tmp.substr(start_delim_idx + 1, end_delim_idx - start_delim_idx - 1);

                // Convert this remaining string to a stream so we can parse our data in doubles
                istringstream data_stream(data_str);
                for (size_t j = 0; j < 7; j++) {
                    double delay;
                    char delim;
                    data_stream >> delay >> delim;
                    myMap[key][0][i][j] = delay;


                }
            }

            // At the end of nested for loop we will have finised reading the 7x7 delays.
            // Increment our database pointer so we can store the next entry
            db_idx++;

            // Sanity check for array size
            if (db_idx >= DB_MAX_SIZE) {
                cout << "ERROR: Insufficient map size for saving data" << endl;
                ifs.close();
                return -1;
            }
        }else if(first_word.compare("output_slew(Timing_7_7)") == 0){
            // Read 3 lines that contain the rest of above match, index 1 and index 2
            string tmp;
            getline(ifs, tmp);
            getline(ifs, tmp);
            getline(ifs, tmp);

            // From here on the next 7 lines will contain our delays
            for (size_t i = 0; i < 7; i++) {
                getline(ifs, tmp);

                // The delays will be between " ". Find the opening ".
                size_t start_delim_idx = tmp.find("\"");

                // Find the closing ".
                // The second argument is where we want to start our search
                // Ignore the first match so we don't get the same index again
                size_t end_delim_idx = tmp.find("\"", start_delim_idx + 1);

                // The second arg in substr in no. of characters, not the ending index
                string data_str = tmp.substr(start_delim_idx + 1, end_delim_idx - start_delim_idx - 1);

                // Convert this remaining string to a stream so we can parse our data in doubles
                istringstream data_stream(data_str);
                for (size_t j = 0; j < 7; j++) {
                    double delay;
                    char delim;
                    data_stream >> delay >> delim;

                    myMap[key][1][i][j] = delay;

                }
            }

            // At the end of nested for loop we will have finised reading the 7x7 delays.
            // Increment our database pointer so we can store the next entry
            db_idx++;

            // Sanity check for array size
            if (db_idx >= DB_MAX_SIZE) {
                cout << "ERROR: Insufficient map size for saving data" << endl;
                ifs.close();
                return -1;
            }
        }
	}
    ifs.close();

	return 0;
}


