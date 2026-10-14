#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Course {
    string courseCode;
    int creditUnits;
    int score;
    char grade;
    double gradePoint;
};

class Student {
private:
    int studentID;
    string name;
    vector<Course> courses;
    double gpa;

    char calculateGrade(int score, double &gp) {
        if (score >= 70) {
            gp = 5.0;
            return 'A';
        } else if (score >= 60) {
            gp = 4.0;
            return 'B';
        } else if (score >= 50) {
            gp = 3.0;
            return 'C';
        } else if (score >= 45) {
            gp = 2.0;
            return 'D';
        } else if (score >= 40) {
            gp = 1.0;
            return 'E';
        } else {
            gp = 0.0;
            return 'F';
        }
    }

public:
    Student() : studentID(0), name(""), gpa(0.0) {}

    Student(int id, string n) : studentID(id), name(n), gpa(0.0) {}

    int getID() const {
        return studentID;
    }

    string getName() const {
        return name;
    }

    double getGPA() const {
        return gpa;
    }

    const vector<Course>& getCourses() const {
        return courses;
    }

    void setName(string n) {
        name = n;
    }

    void addCourse(string code, int units, int score) {
        Course c;
        c.courseCode = code;
        c.creditUnits = units;
        c.score = score;
        c.grade = calculateGrade(score, c.gradePoint);
        courses.push_back(c);
        calculateGPA();
    }

    void clearCourses() {
        courses.clear();
        gpa = 0.0;
    }

    void calculateGPA() {
        if (courses.empty()) {
            gpa = 0.0;
            return;
        }
        int totalCredits = 0;
        double totalQualityPoints = 0.0;
        for (const auto &c : courses) {
            totalCredits += c.creditUnits;
            totalQualityPoints += (c.gradePoint * c.creditUnits);
        }
        if (totalCredits > 0) {
            gpa = totalQualityPoints / totalCredits;
        } else {
            gpa = 0.0;
        }
    }

    void display() const {
        cout << "\n--------------------------------------------------\n";
        cout << "Student ID: " << studentID << "\n";
        cout << "Name:       " << name << "\n";
        cout << "GPA:        " << fixed << setprecision(2) << gpa << "\n";
        cout << "Courses Enrolled:\n";
        if (courses.empty()) {
            cout << "  No courses recorded yet.\n";
        } else {
            cout << setw(12) << left << "Code" 
                 << setw(10) << right << "Units" 
                 << setw(10) << right << "Score" 
                 << setw(10) << right << "Grade" << "\n";
            cout << "--------------------------------------------------\n";
            for (const auto &c : courses) {
                cout << setw(12) << left << c.courseCode 
                     << setw(10) << right << c.creditUnits 
                     << setw(10) << right << c.score 
                     << setw(10) << right << c.grade << "\n";
            }
        }
        cout << "--------------------------------------------------\n";
    }

    void serialize(ofstream &outFile) const {
        outFile.write(reinterpret_cast<const char*>(&studentID), sizeof(studentID));
        size_t nameLength = name.size();
        outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        outFile.write(name.c_str(), nameLength);

        size_t courseCount = courses.size();
        outFile.write(reinterpret_cast<const char*>(&courseCount), sizeof(courseCount));
        for (const auto &c : courses) {
            size_t codeLength = c.courseCode.size();
            outFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));
            outFile.write(c.courseCode.c_str(), codeLength);
            outFile.write(reinterpret_cast<const char*>(&c.creditUnits), sizeof(c.creditUnits));
            outFile.write(reinterpret_cast<const char*>(&c.score), sizeof(c.score));
            outFile.write(reinterpret_cast<const char*>(&c.grade), sizeof(c.grade));
            outFile.write(reinterpret_cast<const char*>(&c.gradePoint), sizeof(c.gradePoint));
        }
        outFile.write(reinterpret_cast<const char*>(&gpa), sizeof(gpa));
    }

    void deserialize(ifstream &inFile) {
        inFile.read(reinterpret_cast<char*>(&studentID), sizeof(studentID));
        size_t nameLength;
        inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        name.resize(nameLength);
        inFile.read(&name[0], nameLength);

        size_t courseCount;
        inFile.read(reinterpret_cast<char*>(&courseCount), sizeof(courseCount));
        courses.resize(courseCount);
        for (size_t i = 0; i < courseCount; ++i) {
            size_t codeLength;
            inFile.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));
            courses[i].courseCode.resize(codeLength);
            inFile.read(&courses[i].courseCode[0], codeLength);
            inFile.read(reinterpret_cast<char*>(&courses[i].creditUnits), sizeof(courses[i].creditUnits));
            inFile.read(reinterpret_cast<char*>(&courses[i].score), sizeof(courses[i].score));
            inFile.read(reinterpret_cast<char*>(&courses[i].grade), sizeof(courses[i].grade));
            inFile.read(reinterpret_cast<char*>(&courses[i].gradePoint), sizeof(courses[i].gradePoint));
        }
        inFile.read(reinterpret_cast<char*>(&gpa), sizeof(gpa));
    }
};

class SystemManager {
private:
    vector<Student> students;
    const string filename = "students_data.dat";

    void loadFromFile() {
        ifstream inFile(filename, ios::binary);
        if (!inFile) return;
        students.clear();
        while (inFile.peek() != EOF) {
            Student s;
            s.deserialize(inFile);
            students.push_back(s);
        }
        inFile.close();
    }

    void saveToFile() {
        ofstream outFile(filename, ios::binary | ios::trunc);
        for (const auto &s : students) {
            s.serialize(outFile);
        }
        outFile.close();
    }

public:
    SystemManager() {
        loadFromFile();
    }

    void addStudent() {
        int id;
        string name;
        cout << "\nEnter Student ID (integer): ";
        cin >> id;

        for (const auto &s : students) {
            if (s.getID() == id) {
                cout << "Error: Student with ID " << id << " already exists.\n";
                return;
            }
        }

        cout << "Enter Student Name: ";
        cin.ignore();
        getline(cin, name);

        Student s(id, name);
        
        char addMore;
        do {
            string code;
            int units, score;
            cout << "  Enter Course Code (e.g., CSC101): ";
            cin >> code;
            cout << "  Enter Credit Units: ";
            cin >> units;
            cout << "  Enter Score (0-100): ";
            cin >> score;

            if (score < 0 || score > 100 || units <= 0) {
                cout << "  Invalid score or credit units. Course not added.\n";
            } else {
                s.addCourse(code, units, score);
                cout << "  Course added successfully.\n";
            }

            cout << "  Add another course for this student? (y/n): ";
            cin >> addMore;
        } while (addMore == 'y' || addMore == 'Y');

        students.push_back(s);
        saveToFile();
        cout << "Student record saved successfully.\n";
    }

    void viewAllStudents() const {
        if (students.empty()) {
            cout << "\nNo student records found in the system.\n";
            return;
        }
        cout << "\n================ STUDENT RECORDS ================\n";
        cout << setw(10) << left << "ID" 
             << setw(25) << left << "Name" 
             << setw(10) << right << "GPA" << "\n";
        cout << "--------------------------------------------------\n";
        for (const auto &s : students) {
            cout << setw(10) << left << s.getID() 
                 << setw(25) << left << s.getName() 
                 << setw(10) << right << fixed << setprecision(2) << s.getGPA() << "\n";
        }
        cout << "==================================================\n";
    }

    void searchStudent() const {
        if (students.empty()) {
            cout << "\nNo student records found.\n";
            return;
        }
        int id;
        cout << "\nEnter Student ID to search: ";
        cin >> id;

        for (const auto &s : students) {
            if (s.getID() == id) {
                s.display();
                return;
            }
        }
        cout << "Student with ID " << id << " not found.\n";
    }

    void updateStudent() {
        if (students.empty()) {
            cout << "\nNo student records found.\n";
            return;
        }
        int id;
        cout << "\nEnter Student ID to update: ";
        cin >> id;

        auto it = find_if(students.begin(), students.end(), [id](const Student &s) {
            return s.getID() == id;
        });

        if (it == students.end()) {
            cout << "Student with ID " << id << " not found.\n";
            return;
        }

        cout << "\nUpdating record for: " << it->getName() << "\n";
        cout << "1. Update Name\n";
        cout << "2. Replace/Add Courses and Scores\n";
        cout << "Enter choice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            string newName;
            cout << "Enter new name: ";
            cin.ignore();
            getline(cin, newName);
            it->setName(newName);
            cout << "Name updated successfully.\n";
        } else if (choice == 2) {
            it->clearCourses();
            char addMore;
            do {
                string code;
                int units, score;
                cout << "  Enter Course Code: ";
                cin >> code;
                cout << "  Enter Credit Units: ";
                cin >> units;
                cout << "  Enter Score (0-100): ";
                cin >> score;

                if (score < 0 || score > 100 || units <= 0) {
                    cout << "  Invalid score or credit units.\n";
                } else {
                    it->addCourse(code, units, score);
                    cout << "  Course added successfully.\n";
                }

                cout << "  Add another course? (y/n): ";
                cin >> addMore;
            } while (addMore == 'y' || addMore == 'Y');
            cout << "Courses updated successfully.\n";
        } else {
            cout << "Invalid choice.\n";
            return;
        }

        saveToFile();
    }

    void deleteStudent() {
        if (students.empty()) {
            cout << "\nNo student records found.\n";
            return;
        }
        int id;
        cout << "\nEnter Student ID to delete: ";
        cin >> id;

        auto it = remove_if(students.begin(), students.end(), [id](const Student &s) {
            return s.getID() == id;
        });

        if (it == students.end()) {
            cout << "Student with ID " << id << " not found.\n";
        } else {
            students.erase(it, students.end());
            saveToFile();
            cout << "Student record deleted successfully.\n";
        }
    }
};

int main() {
    SystemManager manager;
    int choice;

    do {
        cout << "\n=== STUDENT RESULT MANAGEMENT SYSTEM ===" << endl;
        cout << "1. Add Student Record" << endl;
        cout << "2. View All Students" << endl;
        cout << "3. Search Student by ID" << endl;
        cout << "4. Update Student Record" << endl;
        cout << "5. Delete Student Record" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice (1-6): ";
        cin >> choice;

        switch (choice) {
            case 1:
                manager.addStudent();
                break;
            case 2:
                manager.viewAllStudents();
                break;
            case 3:
                manager.searchStudent();
                break;
            case 4:
                manager.updateStudent();
                break;
            case 5:
                manager.deleteStudent();
                break;
            case 6:
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 6." << endl;
        }
    } while (choice != 6);

    return 0;
}
