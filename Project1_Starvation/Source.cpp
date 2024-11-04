#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

const int TOTAL_SIZE = 100000, OS_SIZE = 10000;

class Job
{
private:
	string jobID;
	int jobSize;
	int startingAddress;
	int endingAddress;
	int starvation;
public:
	Job(string jobID = "", int jobSize = 0, int startingAddress = 0, int endingAddress = 0, int starvation = 0) {
		this->jobID = jobID;
		this->jobSize = jobSize;
		this->startingAddress = startingAddress;
		this->endingAddress = endingAddress;
		this->starvation = starvation;
	}
	void setJobID(string jobID) { this->jobID = jobID; }
	void setJobSize(int jobSize) { this->jobSize = jobSize; }
	void setStartingAddress(int startingAddress) { this->startingAddress = startingAddress; }
	void setEndingAddress(int endingAddress) { this->endingAddress = endingAddress; }
	void setStarvation(int starvation) { this->starvation = starvation; }
	int getStartingAddress() const { return startingAddress; }
	int getEndingAddress() const { return endingAddress; }
	int getStarvation() const { return starvation; }
	string getJobID() const { return jobID; }
	int getJobSize() const { return jobSize; }
	void memoryPrint() {
		cout << setw(20) << left << jobID << setw(25) << left << startingAddress << setw(20) << left << jobSize << endl;
	}
	void waitingPrint() {
		cout << setw(20) << left << jobID << setw(25) << left << jobSize << endl;
	}
};

int getInput1(string userInput) {
	while (userInput[0] < 49 || userInput[0] > 51 || userInput.length() > 1) {
		cout << "Wrong option! Please choose again: ";
		getline(cin, userInput);
	}
	return int(userInput[0]) - 48;
}

int getInput2(string userInput) {
	for (int i = 0; i < userInput.size(); i++) {
		if (i == 0) {
			if (userInput[i] < 49 || userInput[i] > 57) {
				cout << "Invalid Job Size!" << endl;
				return -1;
			}
		}
		else {
			if (userInput[i] < 48 || userInput[i] > 57) {
				cout << "Invalid Job Size!" << endl;
				return -1;
			}
		}
	}
	if (stoi(userInput) > TOTAL_SIZE - OS_SIZE) {
		cout << "Job Size is too large!" << endl;
		return -1;
	}
	return stoi(userInput);
}


int freeSpaceCalculate(vector<Job> jobMemory) {
	int freeSpace = TOTAL_SIZE;
	for (int i = 0; i < jobMemory.size(); i++) {
		freeSpace -= jobMemory[i].getJobSize();
	}
	return freeSpace;
}

int startingAddressCalculate(int jobSize, vector<Job> jobMemory) {
	return jobMemory[jobMemory.size() - 1].getEndingAddress() + 1;
}

int print(int freeSpace, vector<Job> jobMemory, vector<Job> jobWaiting) {
	cout << "Memory (total size: 100000 bytes, free space: " << freeSpaceCalculate(jobMemory) << " bytes)" << endl;
	cout << setw(20) << left << "Job" << setw(25) << left << "Starting Adress" << setw(20) << left << "Size (in bytes)" << endl;
	for (int i = 0; i < jobMemory.size(); i++) {
		jobMemory[i].memoryPrint();
	}
	cout << "----------------------------------------" << endl;
	cout << "Wating Queue" << endl;
	cout << setw(20) << left << "Job" << setw(25) << left << "Size (in bytes)" << endl;
	if (jobWaiting.size() == 0) cout << "Empty" << endl;
	else {
		for (int i = 0; i < jobWaiting.size(); i++) {
			jobWaiting[i].waitingPrint();
		}
	}
	cout << "----------------------------------------" << endl;
	cout << "1. Add a Job" << endl;
	cout << "2. Remove a Job" << endl;
	cout << "3. Exit" << endl;
	cout << "Enter your options: ";
	string userInput;
	getline(cin, userInput);
	return getInput1(userInput);
}

string evaluateID(vector<Job> jobMemory, vector<Job> jobWaiting) {
	int newID = 1;
	bool flag = true;
	vector<int> id;
	for (int i = 1; i < jobMemory.size(); i++) {
		id.push_back(stoi(jobMemory[i].getJobID().substr(4)));
	}
	for (int i = 0; i < jobWaiting.size(); i++) {
		id.push_back(stoi(jobWaiting[i].getJobID().substr(4)));
	}
	while (flag) {
		flag = false;
		for (int i = 0; i < id.size(); i++) {
			if (newID == id[i]) {
				newID++;
				flag = true;
				break;
			}
		}
	}

	return "Job_" + to_string(newID);
}

bool checkID(string jobID, vector<Job> jobMemory) {
	for (int i = 1; i < jobMemory.size(); i++) {
		if (jobID == jobMemory[i].getJobID()) {
			return true;
		}
	}
	return false;
}

void compacting(string jobID, vector<Job>& jobMemory) {
	int deletedJobLocation = 0;
	for (int i = 0; i < jobMemory.size(); i++) {
		if (jobID == jobMemory[i].getJobID()) {
			deletedJobLocation = i;
			break;
		}
	}
	for (int i = deletedJobLocation; i < jobMemory.size() - 1; i++) {
		jobMemory[i].setJobID(jobMemory[i + 1].getJobID());
		jobMemory[i].setJobSize(jobMemory[i + 1].getJobSize());
		jobMemory[i].setStartingAddress(jobMemory[i - 1].getEndingAddress() + 1);
		jobMemory[i].setEndingAddress(jobMemory[i].getStartingAddress() + jobMemory[i].getJobSize() - 1);
	}
	jobMemory.pop_back();
}

void memoryUpdated(vector<Job>& jobMemory, vector<Job>& jobWaiting) {
	for (int i = 0; i < jobWaiting.size(); i++) {
		if (freeSpaceCalculate(jobMemory) >= jobWaiting[i].getJobSize()) {
			jobWaiting[i].setStartingAddress(startingAddressCalculate(jobWaiting[i].getJobSize(), jobMemory));
			jobWaiting[i].setEndingAddress(startingAddressCalculate(jobWaiting[i].getJobSize(), jobMemory) + jobWaiting[i].getJobSize() - 1);
			jobMemory.push_back(jobWaiting[i]);
			jobWaiting.erase(jobWaiting.begin() + i);
			i = -1;
		}
	}
}

void starvation(vector<Job>& jobWaiting) {
	for (int i = 0; i < jobWaiting.size(); i++) {
		jobWaiting[i].setStarvation(jobWaiting[i].getStarvation() + 1);
	}
}

bool checkStarvation(vector<Job> jobWaiting) {
	for (int i = 0; i < jobWaiting.size(); i++) {
		if (jobWaiting[i].getStarvation() == 3) return false;
	}
	return true;
}

int main() {
	int freeSpace = TOTAL_SIZE - OS_SIZE, jobSize = 0;
	vector<Job> jobMemory, jobWaiting;
	Job a("OS", OS_SIZE, 0, 9999);
	jobMemory.push_back(a);
	string jobID;
	string jobSizeInput;
	while (true) {
		int userOption = print(freeSpaceCalculate(jobMemory), jobMemory, jobWaiting);
		switch (userOption) {
		case 1:
			if (checkStarvation(jobWaiting)) {
				jobID = evaluateID(jobMemory, jobWaiting);
				cout << "Job ID is " << jobID << endl;
				cout << "Please enter Job Size: ";
				getline(cin, jobSizeInput);
				jobSize = getInput2(jobSizeInput);
				if (jobSize == -1) break;
				a.setJobID(jobID);
				a.setJobSize(jobSize);
				a.setStartingAddress(startingAddressCalculate(jobSize, jobMemory));
				a.setEndingAddress(startingAddressCalculate(jobSize, jobMemory) + jobSize - 1);
				if (jobSize <= freeSpaceCalculate(jobMemory)) {
					starvation(jobWaiting);
					jobMemory.push_back(a);
				}
				else {
					starvation(jobWaiting);
					jobWaiting.push_back(a);
				}
			}
			else {
				cout << "Starvation!" << endl;
			}
			break;
		case 2:
			cout << "Please enter job id that you want to remove: ";
			getline(cin, jobID);
			if (jobID == "OS") {
				cout << "You can't remove the operating system." << endl;
				break;
			}
			else if (!checkID(jobID, jobMemory)) {
				cout << "Invalid job id!" << endl;
				break;
			}
			compacting(jobID, jobMemory);
			memoryUpdated(jobMemory, jobWaiting);
			break;
		case 3:
			exit(0);
		}
	}
	return 0;
}