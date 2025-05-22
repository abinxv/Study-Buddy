#include <iostream>
#include <fstream>
#include <sstream> // QQQQ
#include <string>
#include <ctime>
#include <stack>
#include <vector>
#include <queue>
#include <iomanip> // used in time func
#include <climits> // used in time func
#include <direct.h>
#include <unordered_map>
#include <set>

using namespace std;
//namespace fs = std::filesystem;

// Graph structure using an adjacency list
unordered_map<string, set<string>> flashcardGraph = {
  {"bee", {"fuses", "generator", "machine"}},
  {"chem", {"aspirin", "mot", "hardness"}},
  {"ds", {"algorithm", "oops", "stl"}},
  {"denm", {"derivatives", "interpolation", "infiniteseries"}}
};

//creating task class that stores the structure of a task
class Task {
public:
  string title;
  string subject;
  string topic;
  string description;
  string createdTime;
  string dueTime;
  int priority;
  bool reminded = false;
  Task* next = nullptr;
  Task* prev = nullptr;
};

// declaring undo stack and completed queue globally so we can use thru out the code
stack<Task*> undo;
queue<Task*> completed;

// QQQQQQQQQQQQ
int calculateMinutesUntilDue(const string& dueTimeStr) {
  tm tm = {};
  istringstream ss(dueTimeStr);
  ss >> get_time(&tm, "%Y-%m-%d %H:%M");
  if (ss.fail()) {
    cout << " Invalid due time format. Use YYYY-MM-DD HH:MM\n";
    return INT_MAX; // QQQQQQQQQQQQ
  }
  time_t dueTime = mktime(&tm);
  time_t now = time(nullptr);
  return static_cast<int>(difftime(dueTime, now) / 60); // QQQQQQQQQQQQ
}

// creating new task
void addTask(Task*& head, Task* newTask) {
  if (head == NULL || newTask->priority < head->priority) {
    newTask->next = head;
    if (head) head->prev = newTask;
    head = newTask;
    return;
  }

  Task* curr = head;
  while (curr->next && curr->next->priority <= newTask->priority) {
    curr = curr->next;
  }
  newTask->next = curr->next;
  newTask->prev = curr;
  if (curr->next) curr->next->prev = newTask;
  curr->next = newTask;
}

// deleting task
void deleteTask(Task*& head, const string& title) {
  if (!head) {
    cout << "You have no tasks.\n";
    return;
  }

  Task* curr = head;
  while (curr && curr->title != title) {
    curr = curr->next;
  }

  if (!curr) {
    cout << "Task not found.\n";
    return;
  }

  if (curr->prev) curr->prev->next = curr->next;
  else head = curr->next;

  if (curr->next) curr->next->prev = curr->prev;

  undo.push(curr);
}

// // undo deleting a task (pushing all deleted tasks onto a stack)
// void undoDeletion(Task*& head) {
//   if (undo.empty()) {
//     cout << "You can't undo if you didn't delete any tasks.\n";
//     return;
//   }
//   addTask(head, undo.top());
//   undo.pop();
// }

// recycle bin functionality
void recycleBin(Task*& head) {
  cout << "welcome to the recycle bin!\n1.restore the task\n2.delete the task permanantly\n";
  int binchoice;
  cin >> binchoice;
  cin.ignore(); // to clear input buffer
 
  if (undo.empty()) {
    cout << "Recycle bin is empty.\n";
    return;
  }
 
  if (binchoice == 1) {
    addTask(head, undo.top());
    undo.pop();
    return;
  }
  else if (binchoice == 2) {
    undo.pop();
    return;
  }
  else {
    cout << "Invalid choice!";
    return;
  }
}

// completing a task (pushing all completed tasks into a queue)
void completeTask(Task*& head, const string& title) {
  if (!head) {
    cout << "No tasks to complete.\n";
    return;
  }

  Task* curr = head;
  while (curr && curr->title != title) {
    curr = curr->next;
  }

  if (!curr) {
    cout << "Task not found.\n";
    return;
  }

  if (curr->prev) curr->prev->next = curr->next;
  else head = curr->next;

  if (curr->next) curr->next->prev = curr->prev;

  curr->next = nullptr;
  curr->prev = nullptr;

  completed.push(curr);
  cout << " You have completed the task: " << curr->title << "\n";
}

void viewCompletedTasks() {
  if (completed.empty()) {
    cout << "No completed tasks yet.\n";
    return;
  }

  queue<Task*> temp = completed;
  cout << "\n Completed Tasks:\n";
  while (!temp.empty()) {
    Task* t = temp.front();
    cout << "---------------------------\n";
    cout << "Title: " << t->title << "\n";
    cout << "Subject: " << t->subject << "\n";
    cout << "Topic: " << t->topic << "\n";
    cout << "Description: " << t->description << "\n";
    temp.pop();
  }
}

// displaying all tasks
void displayTasks(Task* head) {
  if (!head) {
    cout << "No tasks.\n";
    return;
  }
  while (head) {
    cout << "---------------------------\n";
    cout << "Title: " << head->title << "\n";
    cout << "Subject: " << head->subject << "\n";
    cout << "Topic: " << head->topic << "\n";
    cout << "Description: " << head->description << "\n";
    cout << "Created: " << head->createdTime << "\n";
    cout << "Due: " << head->dueTime << "\n";
    // cout << "Priority: " << head->priority << "\n";
    head = head->next;
  }
}

// merge sort for sorting tasks alphabetically
void merge(vector<Task*>& tasks, int l, int m, int r) {
  vector<Task*> temp;

  int i = l, j = m + 1;
  while (i <= m && j <= r) {
    if (tasks[i]->title < tasks[j]->title) temp.push_back(tasks[i++]);
    else temp.push_back(tasks[j++]);
  }
  while (i <= m) temp.push_back(tasks[i++]);
  while (j <= r) temp.push_back(tasks[j++]);
  for (int k = 0; k < temp.size(); ++k) tasks[l + k] = temp[k];
}

void mergeSort(vector<Task*>& tasks, int l, int r) {
  if (l >= r) return;
  int m = (r + l) / 2;
  mergeSort(tasks, l, m);
  mergeSort(tasks, m + 1, r);
  merge(tasks, l, m, r);
}

// displaying tasks alphabetically
void displayTasksAlphabetically(Task* head) {
  if (!head) {
    cout << "No tasks.\n";
    return;
  }

  vector<Task*> taskVec;
  Task* curr = head;
  while (curr) {
    taskVec.push_back(curr);
    curr = curr->next;
  }

  mergeSort(taskVec, 0, taskVec.size() - 1);

  cout << "\n--- Tasks (Alphabetical Order) ---\n";
  for (Task* t : taskVec) {
    cout << "---------------------------\n";
    cout << "Title: " << t->title << "\n";
    cout << "Subject: " << t->subject << "\n";
    cout << "Topic: " << t->topic << "\n";
    cout << "Description: " << t->description << "\n";
    cout << "Created: " << t->createdTime << "\n";
    cout << "Due: " << t->dueTime << "\n";
    cout << "Priority: " << t->priority << "\n";
  }
}

// displaying a reminder if the task is due less than 30 mins
void checkReminders(Task* head, int remMsg = 30) {
  Task* curr = head;
  while (curr) {
    int minsLeft = calculateMinutesUntilDue(curr->dueTime);
    if (minsLeft <= remMsg && minsLeft > 0 && !curr->reminded) {
      cout << " Reminder: \"" << curr->title << "\" is due in " << minsLeft << " minutes!\n";
      curr->reminded = true;
    }
    curr = curr->next;
  }
}

// saving all tasks onto another text file so that we can view them later
void saveTasksToFile(Task* head, const string& filename) {
  ofstream out(filename);
  while (head) {
    out << head->title << "|" << head->subject << "|" << head->topic << "|"
        << head->description << "|" << head->createdTime << "|" << head->dueTime << "|" << head->priority << "\n";
    head = head->next;
  }
}

//  IMP CHECK ONCE
Task* loadTasksFromFile() {
  ifstream in("task_data.txt");
  Task* head = nullptr;
  string line;
  while (getline(in, line)) {
    stringstream ss(line); // QQQQQQQQQQQQ
    Task* t = new Task();
    getline(ss, t->title, '|');
    getline(ss, t->subject, '|');
    getline(ss, t->topic, '|');
    getline(ss, t->description, '|');
    getline(ss, t->createdTime, '|');
    getline(ss, t->dueTime, '|');
    ss >> t->priority;
    t->next = nullptr;
    addTask(head, t);
  }
  return head;
}

// displaying the flashcard based on user's choice
void suggestFlashcards(const string& topic, const string &subject) {
  string filename = "flashcards/" + subject + "/" + topic + ".txt";
  ifstream file(filename);
  if (!file) {
    cout << "No flashcards found for this topic.\n";
    return;
  }
  cout << "\n--- Flashcards for topic: " << topic << " ---\n";
  string line;
  while (getline(file, line)) cout << line << "\n";
}

// Add flashcard to graph
void addFlashcardToGraph(const string& subject, const string& topic) {
  flashcardGraph[subject].insert(topic);
}

// Function to add a flashcard
void addFlashcard() {
  string subject, topic, content, line;
  cout << "Enter subject: ";
  getline(cin, subject);
  cout << "Enter topic: ";
  getline(cin, topic);
  cout << "Enter flashcard content (type 'END' on a new line to finish):\n";

  while (true) {
    getline(cin, line);
    if (line == "END") break;
    content += line + "\n";
  }

  string dir = "flashcards/" + subject;
  string filename = dir + "/" + topic + ".txt";

  // Create the directory if it does not exist
  _mkdir(dir.c_str());

  ofstream file(filename, ios::app); // Append to the file
  if (!file) {
    cout << "Error creating flashcard file.\n";
    return;
  }

  file << content;
  cout << "Flashcard added successfully to " << filename << "\n";

  // Add to the graph
  addFlashcardToGraph(subject, topic);
}

// View all available flashcards
void viewFlashcards() {
  if (flashcardGraph.empty()) {
    cout << "No flashcards available.\n";
    return;
  }
  cout << "\n--- Available Flashcards ---\n";
  for (const auto& subject : flashcardGraph) {
    cout << "Subject: " << subject.first << "\n";
    for (const auto& topic : subject.second) {
      cout << "  - Topic: " << topic << "\n";
    }
  }
}

// searching and displaying an individual task and performing operations on it
void searchAndOpenTask(Task*& head) {
  if (!head) {
    cout << "No tasks available.\n";
    return;
  }

  string title;
  cout << "Enter task title to search: ";
  getline(cin, title);

  Task* current = head;
  while (current && current->title != title) current = current->next;

  if (!current) {
    cout << "Task not found.\n";
    return;
  }

  cout << "\n--- Task Found ---\n";
  cout << "Title: " << current->title << "\n";
  cout << "Subject: " << current->subject << "\n";
  cout << "Topic: " << current->topic << "\n";
  cout << "Description: " << current->description << "\n";
  cout << "Created: " << current->createdTime << "\n";
  cout << "Due: " << current->dueTime << "\n";
  cout << "Priority: " << current->priority << "\n";

  cout << "\nChoose an action:\n";
  cout << "1. Complete Task\n";
  cout << "2. Delete Task\n";
  cout << "3. Go Back\n";
  cout << "Choice: ";
  int action;
  cin >> action;
  cin.ignore();

  if (action == 1) completeTask(head, title);
  else if (action == 2) deleteTask(head, title);
  else cout << "Returning to main menu.\n";
}

// edit existing task
void editTask(Task* head) {
  if (!head) {
    cout << "No tasks available.\n";
    return;
  }

  string title;
  cout << "Enter task title to edit: ";
  getline(cin, title);

  Task* curr = head;
  while (curr && curr->title != title) {
    curr = curr->next;
  }

  if (!curr) {
    cout << "Task not found.\n";
    return;
  }

  cout << "\n--- Editing Task: " << curr->title << " ---\n";
  cout << "Select field to edit:\n";
  cout << "1. Subject\n";
  cout << "2. Topic\n";
  cout << "3. Description\n";
  cout << "4. Due Time\n";
  cout << "5. Cancel\n";
  cout << "Choice: ";

  int option;
  cin >> option;
  cin.ignore();

  switch (option) {
    case 1:
      cout << "Old Subject: " << curr->subject << endl;
      cout << "New Subject: ";
      getline(cin, curr->subject);
      break;
    case 2:
      cout << "Old Topic: " << curr->topic << endl;
      cout << "New Topic: ";
      getline(cin, curr->topic);
      break;
    case 3:
      cout << "Old Description: " << curr->description << endl;
      cout << "New Description: ";
      getline(cin, curr->description);
      break;
    case 4:
      cout << "Old Due Time: " << curr->dueTime << endl;
      cout << "New Due Time (YYYY-MM-DD HH:MM): ";
      getline(cin, curr->dueTime);
      curr->priority = calculateMinutesUntilDue(curr->dueTime);  // Recalculate priority
      break;
    case 5:
      cout << "Edit cancelled.\n";
      return;
    default:
      cout << "Invalid option.\n";
      return;
  }
  cout << " Task updated successfully.\n";
}

// main function
// main function
int main() {
  // loading any previous tasks in the text file to our list
  Task* taskList = loadTasksFromFile();

  cout << "\n=================================\n";
  cout << "     WELCOME TO STUDYBUDDY!  \n";
  cout << "=================================\n";
  cout << "Smart planning + Flashcards = Success!\n";

  while (true) {
   
    // checkReminders(taskList);
    cout << "\n\n============ Task Manager ============\n\n";
    cout << "1. Add Task" << endl;
    cout << "2. Delete Task" << endl;
    cout << "3. View All Tasks" << endl;
    cout << "4. Suggest Flashcards" << endl;
    cout << "5. Search Task" << endl;
    cout << "6. View Completed Tasks" << endl;
    cout << "7. Recycle Bin" << endl;
    cout << "8. Edit Task" << endl;
    cout << "9. Add Flashcard" << endl;
    cout << "10. View All Flashcards" << endl;
    cout << "11. Exit" << endl;
    cout << "Choose: ";
   
    int choice;
    cin >> choice;
    cin.ignore();
   
    if (choice == 1) {
      Task* t = new Task();
      cout << "Title: "; getline(cin, t->title);
      cout << "Subject: "; getline(cin, t->subject);
      cout << "Topic: "; getline(cin, t->topic);
      cout << "Description: "; getline(cin, t->description);
      cout << "Due time (YYYY-MM-DD HH:MM): "; getline(cin, t->dueTime);
      t->priority = calculateMinutesUntilDue(t->dueTime);
      cin.ignore();
      time_t now = time(0);
      t->createdTime = ctime(&now);
      addTask(taskList, t);
      string choice;
      cout << "Do you want to check if there are any flashcards and display them: ";
      cin >> choice;
     
      if(choice == "yes" || choice == "Yes" ||choice == "YES" ||choice == "y" ){
        suggestFlashcards(t->topic,t->subject);
      }
      // else if(choice == "no"|| choice == "n" || choice == "No"|| choice == "NO"){
      // }
      cout << "Task added!\n";
     
      int minsLeft = calculateMinutesUntilDue(t->dueTime);
      if (minsLeft <= 30) {
        // Do nothing, will be handled by checkReminders
      }
      else if (minsLeft <= 60) {
        cout << "? Time left: " << minsLeft << " minutes.\n";
      } else {
        double hours = minsLeft / 60.0;
        cout << fixed << setprecision(2);
        cout << "Time left: " << hours << " hours.\n";
      }
    }
    else if (choice == 2) {
      string title;
      cout << "Enter title to delete: ";
      getline(cin, title);
      deleteTask(taskList, title);
    }
    else if (choice == 3) {
      cout << "View by:\n1. Priority\n2. Alphabetically\nChoice: ";
      int view;
      cin >> view;
      cin.ignore();
      if (view == 2)
        displayTasksAlphabetically(taskList);
      else
        displayTasks(taskList);
    }
    else if (choice == 4) {
      string topic;
      string subject;
      cout << "Enter subject: ";
      getline(cin, subject);
      cout << "Enter topic: ";
      getline(cin, topic);
      suggestFlashcards(topic, subject);
    }
    else if (choice == 5) {
      searchAndOpenTask(taskList);
    }
    else if (choice == 6) {
      viewCompletedTasks();
    }
    else if (choice == 7) {
      recycleBin(taskList);
    }
    else if (choice == 8) {
      editTask(taskList);
    }
    else if (choice == 9) {
      addFlashcard();
    }
    else if (choice == 10) {
      viewFlashcards();
    }
    else if (choice == 11) {
      saveTasksToFile(taskList, "task_data.txt");
      cout << "Thank you, I hope you complete your tasks!\nSee you again...";
      break;
    }
    else {
      cout << "Invalid choice.\n";
    }
  }
  return 0;
}