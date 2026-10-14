# Student Result Management System

A robust command-line Student Result Management System written in C++ using Object-Oriented Programming (OOP) principles and binary file handling.

## Features

- **Add Student Records**: Input student ID, name, and multiple courses with credit units and scores.
- **View All Students**: Display a formatted table of all registered students along with their calculated GPAs.
- **Search Individual Students**: Look up specific students by ID to view their full grade report.
- **Update Records**: Modify student names or update course scores and credit units.
- **Delete Records**: Remove student profiles from the system.
- **Automatic Grade & GPA Calculation**: Automatically computes letter grades, grade points, and cumulative GPAs based on scores and credit units.
- **File Persistence**: Automatically saves and loads all records using binary file handling so data remains available after closing the program.

## Getting Started

### Prerequisites
- A modern C++ compiler (supporting C++11 or later, such as `g++`, `clang++`, or MSVC).

### Compilation & Execution
```bash
g++ -std=c++11 main.cpp -o student_system
./student_system
```
