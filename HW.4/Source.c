#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/ //uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct StudentCourseGrade
{
	char courseName[35];
	int grade;
}StudentCourseGrade;

typedef struct Student
{
	char name[35];
	StudentCourseGrade* grades; //dynamic array of courses
	int numberOfCourses;
}Student;


//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);


//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

//free struct student
void freeallocations(Student* s, int numberOfStudents);
int main()
{
	//Part A
	int* coursesPerStudent = NULL;
	int numberOfStudents = 0;
	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	//studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)

	//Part B
	Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("students.bin");

	//add code to free all arrays of struct Student
	freeallocations(transformedStudents, numberOfStudents);
	freeallocations(testReadStudents, numberOfStudents);


	_CrtDumpMemoryLeaks();//uncomment this block to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;

}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	FILE* file = fopen(fileName, "r");
	char c;
	*numberOfStudents = 0;
	char oneline[120];
	do {
		c = getc(file);
		if (c == '\n')
			(*numberOfStudents)++;
	} while (c != EOF);
	if (c != '\n' && *numberOfStudents != 0)
		(*numberOfStudents)++;
	rewind(file);
	*coursesPerStudent = (int*)malloc(*numberOfStudents * sizeof(int));
	int i = 0;
	while (fgets(oneline, 120, file) != NULL)
	{
		coursesPerStudent[0][i] = countPipes(oneline, strlen(oneline));
		i++;
	}
	fclose(file);
}

int countPipes(const char* lineBuffer, int maxCount)
{
	int Count = 0;

	if (lineBuffer == NULL) return -1;
	if (maxCount <= 0) return 0;

	for (int i = 0; i < maxCount; i++) {
		if (lineBuffer[i] == '|')
			Count++;
	}
	return Count;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);
	char*** students = (char***)malloc(*numberOfStudents * sizeof(char**));
	for (int i = 0; i < *numberOfStudents; i++)
		students[i] = (char**)malloc((1 + (coursesPerStudent[0][i]) * 2) * sizeof(char*));
	FILE* file = fopen(fileName, "r");
	char oneline[150];
	int j = 0;
	char* p = NULL;
	char* o = NULL;
	char* k = NULL;
	int flag = 0;
	int whos = 0;
	int steps = 1;
	int counter = 0;
	while (fgets(oneline, 150, file) != NULL)
	{
		char* ptr = strchr(oneline, '\n');
		if (ptr)
			*ptr = '\0';
		p = strtok(oneline, "|");
		while (p != NULL) {
			if (!flag) {
				students[counter][j] = (char*)malloc(strlen(p) * sizeof(char) + 1);
				strcpy(students[counter][j], p);
				flag = 1;
				j++;
			}
			else {
				students[counter][j] = (char*)malloc(strlen(p) * sizeof(char) + 1);
				strcpy(students[counter][j], p);
				j += 2;
			}
			p = strtok(NULL, "|");
		}
		int length = 1 + coursesPerStudent[0][whos] * 2;
		while (steps < length) {
			o = students[counter][steps];
			p = strtok(o, ",");
			char* temp = (char*)malloc(strlen(p) * sizeof(char) + 1);
			strcpy(temp, p);
			p = strtok(NULL, ",");
			students[counter][steps + 1] = (char*)malloc(strlen(p) * sizeof(char) + 1);
			strcpy(students[counter][steps + 1], p);
			students[counter][steps] = temp;
			steps += 2;
		}
		steps = 1;
		whos++;
		flag = 0;
		j = 0;
		counter++;
	}
	fclose(file);
	return students;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	if (students != NULL && numberOfStudents > 0 && factor >= -20 && factor <= 20) {
		for (int i = 0; i < numberOfStudents; i++) {
			for (int j = 1; j < coursesPerStudent[i] * 2; j += 2)
				if (strcmp(courseName, students[i][j]) == 0) {
					int grade = atoi(students[i][j + 1]);
					grade += factor;
					if (grade > 100)
						grade = 100;
					else if (grade < 0)
						grade = 0;
					char grade_ascii[20];
					_itoa(grade, grade_ascii, 10);
					free(students[i][j + 1]);
					students[i][j + 1] = (char*)malloc(strlen(grade_ascii) * sizeof(char) + 1);
					strcpy(students[i][j + 1], grade_ascii);
					break;
				}
		}
	}
}

void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
	
	for (int i = 0; i < numberOfStudents; i++)
	{
		printf("name: %s\n*********\n", students[i][0]);
		for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
		{
			printf("course: %s\n", students[i][j]);
			printf("grade: %s\n", students[i][j + 1]);
		}
		printf("\n");
	}
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	if (students != NULL) {
		FILE* fi = fopen("studentList.txt", "w");
		for (int i = 0; i < numberOfStudents; i++) {
			fprintf(fi, "%s|", students[i][0]);
			free(students[i][0]);
			int j = 1;
			for (j; j < coursesPerStudent[i] * 2 - 1; j += 2) {
				fprintf(fi, "%s,%s|", students[i][j], students[i][j + 1]);
				free(students[i][j]);
				free(students[i][j + 1]);
			}
			fprintf(fi, "%s,%s", students[i][j], students[i][j + 1]);
			free(students[i][j]);
			if (i != numberOfStudents - 1)
				fprintf(fi, "\n");
		}
		fclose(fi);
		for (int i = 0; i < numberOfStudents; i++)
			free(students[i]);
		free(coursesPerStudent);
		free(students);
	}
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	if (fileName != NULL && students != NULL) {
		FILE* f = fopen(fileName, "wb");
		fwrite(&numberOfStudents, sizeof(numberOfStudents), 1, f);
		for (int i = 0; i < numberOfStudents; i++) {
			fwrite(students[i].name, 35, 1, f);
			fwrite(&students[i].numberOfCourses, sizeof(int), 1, f);
			fwrite(students[i].grades, sizeof(StudentCourseGrade), students[i].numberOfCourses, f);
		}
		fclose(f);
	}

}

Student* readFromBinFile(const char* fileName)
{
	if (fileName != NULL) {
		FILE* f = fopen(fileName, "rb");
		int numberofstudents;
		fread(&numberofstudents, sizeof(numberofstudents), 1, f);
		Student* st = (Student*)malloc(numberofstudents * sizeof(Student));
		for (int i = 0; i < numberofstudents; i++) {
			fread(st[i].name, 35, 1, f);
			fread(&st[i].numberOfCourses, sizeof(int), 1, f);
			st[i].grades = (StudentCourseGrade*)malloc(st[i].numberOfCourses * sizeof(StudentCourseGrade));
			fread(st[i].grades, sizeof(StudentCourseGrade), st[i].numberOfCourses, f);
		}
		fclose(f);
		return st;
	}
	else
		return NULL;
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	if (students != NULL) {
		Student* st = (Student*)malloc(numberOfStudents * sizeof(Student));
		for (int i = 0; i < numberOfStudents; i++) {
			strcpy(st[i].name, students[i][0]);
			st[i].numberOfCourses = coursesPerStudent[i];
			st[i].grades = (StudentCourseGrade*)malloc(coursesPerStudent[i] * sizeof(StudentCourseGrade));
			int k = 0;
			for (int j = 1; j < coursesPerStudent[i] * 2; j += 2) {
				strcpy(st[i].grades[k].courseName, students[i][j]);
				st[i].grades[k].grade = atoi(students[i][j + 1]);
				k++;
			}
		}
		return st;
	}
	return NULL;
}
void freeallocations(Student* s, int numberOfStudents) {
	if (s != NULL) {
		for (int i = 0; i < numberOfStudents; i++) {
			free(s[i].grades);
		}
		free(s);
	}
}

