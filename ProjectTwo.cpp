// ProjectTwo.cpp (filename-friendly version)
// CS 300: Data Structures and Algorithms
// Author: Kyle Dauk
// Advising Assistance Program - Project Two
// Implements course lookup and sorting using a Binary Search Tree
// ===============================================================
// This version accepts file names with SPACES and optional QUOTES.
// Example inputs that all work now:
//   CS 300 ABCU_Advising_Program_Input.csv
//   "CS 300 ABCU_Advising_Program_Input.csv"
// ===============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <cctype>

using namespace std;

// =========================
// Helpers for input cleanup
// =========================
static inline string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) start++;
    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

static inline string strip_surrounding_quotes(const string& s) {
    if (s.size() >= 2) {
        char first = s.front();
        char last  = s.back();
        if ((first == '"'  && last == '"') ||
            (first == '\'' && last == '\'')) {
            return s.substr(1, s.size() - 2);
        }
    }
    return s;
}

// =========================
// Course Struct Definition
// =========================
struct Course {
    string courseId;
    string title;
    vector<string> prerequisites;
};

// =========================
// BST Node Definition
// =========================
struct Node {
    Course course;
    Node* left;
    Node* right;
    Node(Course c) : course(c), left(nullptr), right(nullptr) {}
};

// =========================
// Binary Search Tree Class
// =========================
class BinarySearchTree {
private:
    Node* root;

    Node* addNode(Node* node, const Course& course) {
        if (node == nullptr) {
            return new Node(course);
        }
        if (course.courseId < node->course.courseId) {
            node->left = addNode(node->left, course);
        } else {
            node->right = addNode(node->right, course);
        }
        return node;
    }

    void inOrder(Node* node) const {
        if (node != nullptr) {
            inOrder(node->left);
            cout << node->course.courseId << ", " << node->course.title << endl;
            inOrder(node->right);
        }
    }

    Node* search(Node* node, const string& courseId) const {
        if (node == nullptr) return nullptr;
        if (node->course.courseId == courseId) return node;
        if (courseId < node->course.courseId) return search(node->left, courseId);
        else return search(node->right, courseId);
    }

    void deleteTree(Node* node) {
        if (node == nullptr) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    BinarySearchTree() : root(nullptr) {}
    ~BinarySearchTree() { Clear(); }

    void Insert(const Course& course) { root = addNode(root, course); }

    void PrintCourseList() const {
        cout << "\nHere is a sample schedule:\n" << endl;
        inOrder(root);
        cout << endl;
    }

    void PrintCourse(string courseId) const {
        // normalize to upper
        transform(courseId.begin(), courseId.end(), courseId.begin(), ::toupper);
        Node* node = search(root, courseId);
        if (node == nullptr) {
            cout << "Course not found." << endl;
            return;
        }
        cout << node->course.courseId << ", " << node->course.title << endl;
        cout << "Prerequisites: ";
        if (node->course.prerequisites.empty()) {
            cout << "None" << endl;
        } else {
            for (size_t i = 0; i < node->course.prerequisites.size(); ++i) {
                cout << node->course.prerequisites[i];
                if (i + 1 < node->course.prerequisites.size()) cout << ", ";
            }
            cout << endl;
        }
        cout << endl;
    }

    void Clear() { deleteTree(root); root = nullptr; }
};

// =========================
// Load Courses from CSV File
// =========================
vector<Course> loadCourses(const string& filename) {
    vector<Course> courses;
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return courses;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string courseId, title, prereq;
        vector<string> prereqs;

        getline(ss, courseId, ',');
        getline(ss, title, ',');

        // Trim fields in case csv has spaces
        courseId = trim(courseId);
        title    = trim(title);

        while (getline(ss, prereq, ',')) {
            prereq = trim(prereq);
            if (!prereq.empty()) prereqs.push_back(prereq);
        }

        Course course{courseId, title, prereqs};
        courses.push_back(course);
    }
    return courses;
}

// =========================
// Main Program
// =========================
int main() {
    BinarySearchTree bst;
    vector<Course> courses;
    bool dataLoaded = false;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << "\n1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "\nWhat would you like to do? ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input. Please enter a number." << endl;
            continue;
        }

        switch (choice) {
            case 1: {
                cout << "\nEnter the file name to load (e.g., CS 300 ABCU_Advising_Program_Input.csv): ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush newline left by previous cin
                string filename;
                getline(cin, filename);
                filename = strip_surrounding_quotes(trim(filename)); // handle quotes & spaces

                courses = loadCourses(filename);
                if (courses.empty()) {
                    cout << "No courses loaded. Check the file and try again." << endl;
                    break;
                }

                bst.Clear();
                for (const Course& c : courses) bst.Insert(c);
                dataLoaded = true;
                cout << "Courses loaded successfully." << endl;
                break;
            }
            case 2:
                if (!dataLoaded) {
                    cout << "\nPlease load the data structure first (Option 1)." << endl;
                } else {
                    bst.PrintCourseList();
                }
                break;
            case 3:
                if (!dataLoaded) {
                    cout << "\nPlease load the data structure first (Option 1)." << endl;
                } else {
                    cout << "\nWhat course do you want to know about? ";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ensure clean line read
                    string courseId;
                    getline(cin, courseId);
                    courseId = trim(courseId);
                    bst.PrintCourse(courseId);
                }
                break;
            case 9:
                cout << "\nThank you for using the course planner!" << endl;
                break;
            default:
                cout << "\n" << choice << " is not a valid option." << endl;
                break;
        }
    }
    return 0;
}
