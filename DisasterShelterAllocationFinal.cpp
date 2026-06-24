#include <iostream>
#include <string>
#include <climits>
#include <iomanip>
#include <ctime>
#include <fstream>
using namespace std;

// ==================== CONSTANTS ====================
const int MAX_SHELTERS = 10;
const int MAX_LOCATIONS = 15;
const int MAX_QUEUE = 50;
const int MAX_ALLOCATED = 200;
const int INF = INT_MAX;

// File names
const string SHELTERS_FILE = "shelters.dat";
const string CRITICAL_QUEUE_FILE = "critical_queue.dat";
const string NORMAL_QUEUE_FILE = "normal_queue.dat";
const string ALLOCATED_FILE = "allocated.dat";
const string SYSTEM_FILE = "system.dat";

// ==================== SHELTER STRUCTURE ====================
struct Shelter {
    string name;
    int capacity;
    int currentOccupants;
    int locationID;
    bool isActive;
    string contactNumber;
};

// ==================== VICTIM STRUCTURE ====================
struct Victim {
    string name;
    int severity;
    int currentLocation;
    string contactNumber;
    int registrationTime;
    bool hasSpecialNeeds;
    string specialNeeds;
};

// ==================== ALLOCATED VICTIM RECORD ====================
struct AllocatedVictim {
    Victim victim;
    int shelterIndex;
    int allocationTime;
    bool isActive;
};

// ==================== MIN HEAP FOR CRITICAL VICTIMS ====================
class MinHeap {
private:
    Victim heap[MAX_QUEUE];
    int size;
    
    void heapifyUp(int index) {
        if (index == 0) return;
        int parent = (index - 1) / 2;
        
        if (heap[index].severity > heap[parent].severity) {
            swap(heap[index], heap[parent]);
            heapifyUp(parent);
        }
    }
    
    void heapifyDown(int index) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;
        
        if (left < size && heap[left].severity > heap[largest].severity)
            largest = left;
        if (right < size && heap[right].severity > heap[largest].severity)
            largest = right;
            
        if (largest != index) {
            swap(heap[index], heap[largest]);
            heapifyDown(largest);
        }
    }
    
public:
    MinHeap() : size(0) {}
    
    void insert(Victim v) {
        if (size >= MAX_QUEUE) {
            cout << "Critical queue is full!" << endl;
            return;
        }
        heap[size] = v;
        heapifyUp(size);
        size++;
    }
    
    Victim extractMax() {
        if (size == 0) {
            Victim empty = {"", 0, 0, "", 0, false, ""};
            return empty;
        }
        Victim root = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
        return root;
    }
    
    Victim peek() {
        if (size == 0) {
            Victim empty = {"", 0, 0, "", 0, false, ""};
            return empty;
        }
        return heap[0];
    }
    
    bool isEmpty() { return size == 0; }
    int getSize() { return size; }
    
    void display() {
        if (size == 0) {
            cout << "No critical victims waiting.\n";
            return;
        }
        cout << "\n=== Critical Victims Queue ===\n";
        cout << left << setw(20) << "Name" << setw(10) << "Severity" 
             << setw(12) << "Location" << setw(15) << "Contact" << "Special Needs\n";
        cout << string(80, '-') << endl;
        for (int i = 0; i < size; i++) {
            cout << left << setw(20) << heap[i].name 
                 << setw(10) << heap[i].severity 
                 << setw(12) << heap[i].currentLocation
                 << setw(15) << heap[i].contactNumber
                 << (heap[i].hasSpecialNeeds ? heap[i].specialNeeds : "None") << endl;
        }
    }
    
    bool searchVictim(string name) {
        for (int i = 0; i < size; i++) {
            if (heap[i].name == name) {
                cout << "\n✓ Found in Critical Queue:\n";
                cout << "   Name: " << heap[i].name << endl;
                cout << "   Severity: " << heap[i].severity << endl;
                cout << "   Location: " << heap[i].currentLocation << endl;
                cout << "   Contact: " << heap[i].contactNumber << endl;
                if (heap[i].hasSpecialNeeds)
                    cout << "   Special Needs: " << heap[i].specialNeeds << endl;
                return true;
            }
        }
        return false;
    }
    
    // File operations
    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cout << "⚠ Warning: Could not save critical queue to file.\n";
            return;
        }
        
        file << size << endl;
        for (int i = 0; i < size; i++) {
            file << heap[i].name << "|"
                 << heap[i].severity << "|"
                 << heap[i].currentLocation << "|"
                 << heap[i].contactNumber << "|"
                 << heap[i].registrationTime << "|"
                 << heap[i].hasSpecialNeeds << "|"
                 << heap[i].specialNeeds << endl;
        }
        file.close();
    }
    
    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) return;
        
        size = 0;
        int count;
        file >> count;
        file.ignore();
        
        for (int i = 0; i < count && i < MAX_QUEUE; i++) {
            string line;
            getline(file, line);
            
            size_t pos = 0;
            string token;
            int field = 0;
            
            while ((pos = line.find('|')) != string::npos) {
                token = line.substr(0, pos);
                
                switch(field) {
                    case 0: heap[size].name = token; break;
                    case 1: heap[size].severity = stoi(token); break;
                    case 2: heap[size].currentLocation = stoi(token); break;
                    case 3: heap[size].contactNumber = token; break;
                    case 4: heap[size].registrationTime = stoi(token); break;
                    case 5: heap[size].hasSpecialNeeds = (token == "1"); break;
                }
                
                line.erase(0, pos + 1);
                field++;
            }
            heap[size].specialNeeds = line;
            size++;
        }
        
        file.close();
    }
};

// ==================== SIMPLE QUEUE FOR NORMAL VICTIMS ====================
class Queue {
private:
    Victim queue[MAX_QUEUE];
    int front, rear, size;
    
public:
    Queue() : front(0), rear(-1), size(0) {}
    
    void enqueue(Victim v) {
        if (size >= MAX_QUEUE) {
            cout << "Waiting queue is full!" << endl;
            return;
        }
        rear = (rear + 1) % MAX_QUEUE;
        queue[rear] = v;
        size++;
    }
    
    Victim dequeue() {
        if (size == 0) {
            Victim empty = {"", 0, 0, "", 0, false, ""};
            return empty;
        }
        Victim v = queue[front];
        front = (front + 1) % MAX_QUEUE;
        size--;
        return v;
    }
    
    Victim peek() {
        if (size == 0) {
            Victim empty = {"", 0, 0, "", 0, false, ""};
            return empty;
        }
        return queue[front];
    }
    
    bool isEmpty() { return size == 0; }
    int getSize() { return size; }
    
    void display() {
        if (size == 0) {
            cout << "No victims in waiting queue.\n";
            return;
        }
        cout << "\n=== Normal Waiting Queue ===\n";
        cout << left << setw(20) << "Name" << setw(10) << "Severity" 
             << setw(12) << "Location" << setw(15) << "Contact" << "Special Needs\n";
        cout << string(80, '-') << endl;
        int idx = front;
        for (int i = 0; i < size; i++) {
            cout << left << setw(20) << queue[idx].name 
                 << setw(10) << queue[idx].severity 
                 << setw(12) << queue[idx].currentLocation
                 << setw(15) << queue[idx].contactNumber
                 << (queue[idx].hasSpecialNeeds ? queue[idx].specialNeeds : "None") << endl;
            idx = (idx + 1) % MAX_QUEUE;
        }
    }
    
    bool searchVictim(string name) {
        int idx = front;
        for (int i = 0; i < size; i++) {
            if (queue[idx].name == name) {
                cout << "\n✓ Found in Normal Queue:\n";
                cout << "   Name: " << queue[idx].name << endl;
                cout << "   Severity: " << queue[idx].severity << endl;
                cout << "   Location: " << queue[idx].currentLocation << endl;
                cout << "   Contact: " << queue[idx].contactNumber << endl;
                if (queue[idx].hasSpecialNeeds)
                    cout << "   Special Needs: " << queue[idx].specialNeeds << endl;
                return true;
            }
            idx = (idx + 1) % MAX_QUEUE;
        }
        return false;
    }
    
    // File operations
    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cout << "⚠ Warning: Could not save normal queue to file.\n";
            return;
        }
        
        file << size << "|" << front << "|" << rear << endl;
        
        int idx = front;
        for (int i = 0; i < size; i++) {
            file << queue[idx].name << "|"
                 << queue[idx].severity << "|"
                 << queue[idx].currentLocation << "|"
                 << queue[idx].contactNumber << "|"
                 << queue[idx].registrationTime << "|"
                 << queue[idx].hasSpecialNeeds << "|"
                 << queue[idx].specialNeeds << endl;
            idx = (idx + 1) % MAX_QUEUE;
        }
        file.close();
    }
    
    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) return;
        
        string line;
        getline(file, line);
        
        size_t pos = 0;
        size = stoi(line.substr(0, line.find('|')));
        line.erase(0, line.find('|') + 1);
        front = stoi(line.substr(0, line.find('|')));
        line.erase(0, line.find('|') + 1);
        rear = stoi(line);
        
        int idx = front;
        for (int i = 0; i < size; i++) {
            getline(file, line);
            
            pos = 0;
            int field = 0;
            
            while ((pos = line.find('|')) != string::npos) {
                string token = line.substr(0, pos);
                
                switch(field) {
                    case 0: queue[idx].name = token; break;
                    case 1: queue[idx].severity = stoi(token); break;
                    case 2: queue[idx].currentLocation = stoi(token); break;
                    case 3: queue[idx].contactNumber = token; break;
                    case 4: queue[idx].registrationTime = stoi(token); break;
                    case 5: queue[idx].hasSpecialNeeds = (token == "1"); break;
                }
                
                line.erase(0, pos + 1);
                field++;
            }
            queue[idx].specialNeeds = line;
            idx = (idx + 1) % MAX_QUEUE;
        }
        
        file.close();
    }
};

// ==================== MAIN SYSTEM CLASS ====================
class DisasterShelterSystem {
private:
    Shelter shelters[MAX_SHELTERS];
    int shelterCount;
    
    int graph[MAX_LOCATIONS][MAX_LOCATIONS];
    int locationCount;
    
    MinHeap criticalQueue;
    Queue normalQueue;
    
    AllocatedVictim allocatedVictims[MAX_ALLOCATED];
    int allocatedCount;
    
    int currentTime;
    
    // Dijkstra's Algorithm
    int findShortestDistance(int source, int destination) {
        int dist[MAX_LOCATIONS];
        bool visited[MAX_LOCATIONS] = {false};
        
        for (int i = 0; i < locationCount; i++)
            dist[i] = INF;
        
        dist[source] = 0;
        
        for (int count = 0; count < locationCount - 1; count++) {
            int minDist = INF, minIndex = -1;
            
            for (int v = 0; v < locationCount; v++) {
                if (!visited[v] && dist[v] < minDist) {
                    minDist = dist[v];
                    minIndex = v;
                }
            }
            
            if (minIndex == -1) break;
            visited[minIndex] = true;
            
            for (int v = 0; v < locationCount; v++) {
                if (!visited[v] && graph[minIndex][v] != INF && 
                    dist[minIndex] != INF &&
                    dist[minIndex] + graph[minIndex][v] < dist[v]) {
                    dist[v] = dist[minIndex] + graph[minIndex][v];
                }
            }
        }
        
        return dist[destination];
    }
    
    // Find nearest available shelter
    int findNearestShelter(int victimLocation, bool requiresSpecialNeeds = false) {
        int nearestShelter = -1;
        int minDistance = INF;
        
        for (int i = 0; i < shelterCount; i++) {
            if (shelters[i].isActive && 
                shelters[i].currentOccupants < shelters[i].capacity) {
                
                int distance = findShortestDistance(victimLocation, shelters[i].locationID);
                
                if (distance < minDistance) {
                    minDistance = distance;
                    nearestShelter = i;
                }
            }
        }
        
        return nearestShelter;
    }
    
public:
    DisasterShelterSystem() : shelterCount(0), locationCount(0), allocatedCount(0), currentTime(0) {
        for (int i = 0; i < MAX_LOCATIONS; i++)
            for (int j = 0; j < MAX_LOCATIONS; j++)
                graph[i][j] = (i == j) ? 0 : INF;
        
        for (int i = 0; i < MAX_ALLOCATED; i++)
            allocatedVictims[i].isActive = false;
    }
    
    // ==================== FILE OPERATIONS ====================
    void saveAllData() {
        saveShelters();
        saveSystemData();
        saveAllocatedVictims();
        criticalQueue.saveToFile(CRITICAL_QUEUE_FILE);
        normalQueue.saveToFile(NORMAL_QUEUE_FILE);
        cout << "💾 All data saved successfully!\n";
    }
    
    void loadAllData() {
        loadSystemData();
        loadShelters();
        loadAllocatedVictims();
        criticalQueue.loadFromFile(CRITICAL_QUEUE_FILE);
        normalQueue.loadFromFile(NORMAL_QUEUE_FILE);
    }
    
    void saveShelters() {
        ofstream file(SHELTERS_FILE);
        if (!file) {
            cout << "⚠ Warning: Could not save shelters to file.\n";
            return;
        }
        
        file << shelterCount << endl;
        for (int i = 0; i < shelterCount; i++) {
            file << shelters[i].name << "|"
                 << shelters[i].capacity << "|"
                 << shelters[i].currentOccupants << "|"
                 << shelters[i].locationID << "|"
                 << shelters[i].isActive << "|"
                 << shelters[i].contactNumber << endl;
        }
        file.close();
    }
    
    void loadShelters() {
        ifstream file(SHELTERS_FILE);
        if (!file) return;
        
        file >> shelterCount;
        file.ignore();
        
        for (int i = 0; i < shelterCount; i++) {
            string line;
            getline(file, line);
            
            size_t pos = 0;
            int field = 0;
            
            while ((pos = line.find('|')) != string::npos) {
                string token = line.substr(0, pos);
                
                switch(field) {
                    case 0: shelters[i].name = token; break;
                    case 1: shelters[i].capacity = stoi(token); break;
                    case 2: shelters[i].currentOccupants = stoi(token); break;
                    case 3: shelters[i].locationID = stoi(token); break;
                    case 4: shelters[i].isActive = (token == "1"); break;
                }
                
                line.erase(0, pos + 1);
                field++;
            }
            shelters[i].contactNumber = line;
        }
        
        file.close();
    }
    
    void saveSystemData() {
        ofstream file(SYSTEM_FILE);
        if (!file) {
            cout << "⚠ Warning: Could not save system data to file.\n";
            return;
        }
        
        file << locationCount << "|" << currentTime << endl;
        
        // Save graph
        for (int i = 0; i < locationCount; i++) {
            for (int j = 0; j < locationCount; j++) {
                if (graph[i][j] != INF)
                    file << graph[i][j];
                else
                    file << "INF";
                
                if (j < locationCount - 1) file << " ";
            }
            file << endl;
        }
        
        file.close();
    }
    
    void loadSystemData() {
        ifstream file(SYSTEM_FILE);
        if (!file) return;
        
        string line;
        getline(file, line);
        
        locationCount = stoi(line.substr(0, line.find('|')));
        line.erase(0, line.find('|') + 1);
        currentTime = stoi(line);
        
        // Load graph
        for (int i = 0; i < locationCount; i++) {
            getline(file, line);
            size_t pos = 0;
            int j = 0;
            
            while ((pos = line.find(' ')) != string::npos || !line.empty()) {
                string token;
                if (pos != string::npos) {
                    token = line.substr(0, pos);
                    line.erase(0, pos + 1);
                } else {
                    token = line;
                    line = "";
                }
                
                if (token == "INF")
                    graph[i][j] = INF;
                else
                    graph[i][j] = stoi(token);
                
                j++;
                if (j >= locationCount) break;
            }
        }
        
        file.close();
    }
    
    void saveAllocatedVictims() {
        ofstream file(ALLOCATED_FILE);
        if (!file) {
            cout << "⚠ Warning: Could not save allocated victims to file.\n";
            return;
        }
        
        file << allocatedCount << endl;
        for (int i = 0; i < allocatedCount; i++) {
            file << allocatedVictims[i].victim.name << "|"
                 << allocatedVictims[i].victim.severity << "|"
                 << allocatedVictims[i].victim.currentLocation << "|"
                 << allocatedVictims[i].victim.contactNumber << "|"
                 << allocatedVictims[i].victim.registrationTime << "|"
                 << allocatedVictims[i].victim.hasSpecialNeeds << "|"
                 << allocatedVictims[i].victim.specialNeeds << "|"
                 << allocatedVictims[i].shelterIndex << "|"
                 << allocatedVictims[i].allocationTime << "|"
                 << allocatedVictims[i].isActive << endl;
        }
        file.close();
    }
    
    void loadAllocatedVictims() {
        ifstream file(ALLOCATED_FILE);
        if (!file) return;
        
        file >> allocatedCount;
        file.ignore();
        
        for (int i = 0; i < allocatedCount; i++) {
            string line;
            getline(file, line);
            
            size_t pos = 0;
            int field = 0;
            
            while ((pos = line.find('|')) != string::npos) {
                string token = line.substr(0, pos);
                
                switch(field) {
                    case 0: allocatedVictims[i].victim.name = token; break;
                    case 1: allocatedVictims[i].victim.severity = stoi(token); break;
                    case 2: allocatedVictims[i].victim.currentLocation = stoi(token); break;
                    case 3: allocatedVictims[i].victim.contactNumber = token; break;
                    case 4: allocatedVictims[i].victim.registrationTime = stoi(token); break;
                    case 5: allocatedVictims[i].victim.hasSpecialNeeds = (token == "1"); break;
                    case 6: allocatedVictims[i].victim.specialNeeds = token; break;
                    case 7: allocatedVictims[i].shelterIndex = stoi(token); break;
                    case 8: allocatedVictims[i].allocationTime = stoi(token); break;
                }
                
                line.erase(0, pos + 1);
                field++;
            }
            allocatedVictims[i].isActive = (line == "1");
        }
        
        file.close();
    }
    
    // ==================== SETUP ====================
    void setupSystem() {
        // Check if data files exist
        ifstream testFile(SYSTEM_FILE);
        if (testFile) {
            testFile.close();
            cout << "\n📂 Existing data found! Loading from files...\n";
            loadAllData();
            cout << "✓ Data loaded successfully!\n";
            cout << "  - " << shelterCount << " shelters\n";
            cout << "  - " << criticalQueue.getSize() << " critical victims in queue\n";
            cout << "  - " << normalQueue.getSize() << " normal victims in queue\n";
            cout << "  - " << allocatedCount << " total allocations\n";
        } else {
            cout << "\n📋 No existing data found. Starting with empty system.\n";
            cout << "⚠ Please configure the system:\n";
            cout << "  1. Set up graph (locations and distances)\n";
            cout << "  2. Add shelters\n";
            cout << "  3. Then start registering victims\n";
        }
    }
    
    // ==================== GRAPH SETUP ====================
    void setupGraph() {
        cout << "\n--- Setup City Graph ---\n";
        cout << "Enter number of locations (1-" << MAX_LOCATIONS << "): ";
        cin >> locationCount;
        
        while (locationCount < 1 || locationCount > MAX_LOCATIONS) {
            cout << "Invalid! Enter between 1-" << MAX_LOCATIONS << ": ";
            cin >> locationCount;
        }
        
        cout << "\nEnter distances between locations:\n";
        cout << "(Enter 0 for same location, -1 for no direct path)\n\n";
        
        for (int i = 0; i < locationCount; i++) {
            for (int j = i + 1; j < locationCount; j++) {
                int dist;
                cout << "Distance from Location " << i << " to Location " << j << ": ";
                cin >> dist;
                
                if (dist == -1) {
                    graph[i][j] = INF;
                    graph[j][i] = INF;
                } else {
                    graph[i][j] = dist;
                    graph[j][i] = dist;
                }
            }
        }
        
        cout << "\n✓ Graph configured successfully!\n";
        cout << "  - " << locationCount << " locations set up\n";
        saveAllData();
    }
    
    void addShelter(string name, int capacity, int location, string contact) {
        if (shelterCount >= MAX_SHELTERS) {
            cout << "Cannot add more shelters!" << endl;
            return;
        }
        shelters[shelterCount].name = name;
        shelters[shelterCount].capacity = capacity;
        shelters[shelterCount].currentOccupants = 0;
        shelters[shelterCount].locationID = location;
        shelters[shelterCount].isActive = true;
        shelters[shelterCount].contactNumber = contact;
        shelterCount++;
    }
    
    // ==================== VICTIM REGISTRATION ====================
    void registerVictim() {
        if (locationCount == 0) {
            cout << "\n⚠ Error: Graph not configured! Please setup graph first (Option 15).\n";
            return;
        }
        
        if (shelterCount == 0) {
            cout << "\n⚠ Warning: No shelters available! Please add shelters first (Option 6).\n";
            return;
        }
        
        Victim v;
        cout << "\n--- Register New Victim ---\n";
        cout << "Enter victim name: ";
        cin.ignore();
        getline(cin, v.name);
        
        cout << "Enter contact number: ";
        getline(cin, v.contactNumber);
        
        cout << "Enter severity (1-10, 10 being most critical): ";
        cin >> v.severity;
        
        while (v.severity < 1 || v.severity > 10) {
            cout << "Invalid! Enter severity between 1-10: ";
            cin >> v.severity;
        }
        
        cout << "Enter current location (0-" << locationCount-1 << "): ";
        cin >> v.currentLocation;
        
        while (v.currentLocation < 0 || v.currentLocation >= locationCount) {
            cout << "Invalid! Enter location between 0-" << locationCount-1 << ": ";
            cin >> v.currentLocation;
        }
        
        cout << "Does victim have special needs? (1=Yes, 0=No): ";
        int choice;
        cin >> choice;
        v.hasSpecialNeeds = (choice == 1);
        
        if (v.hasSpecialNeeds) {
            cout << "Enter special needs details: ";
            cin.ignore();
            getline(cin, v.specialNeeds);
        }
        
        v.registrationTime = currentTime++;
        
        // Add to appropriate queue based on severity
        if (v.severity >= 7) {
            criticalQueue.insert(v);
            cout << "\n🚨 CRITICAL CASE - Added to priority queue\n";
        } else {
            normalQueue.enqueue(v);
            cout << "\n✓ Added to normal waiting queue\n";
        }
        
        cout << "   Victim: " << v.name << endl;
        cout << "   Severity: " << v.severity << endl;
        cout << "   Queue Position: " << (v.severity >= 7 ? "Critical Priority" : "Normal") << endl;
        
        saveAllData();
    }
    
    // ==================== PROCESS WAITING QUEUES ====================
    void processWaitingQueues() {
        int criticalProcessed = 0, normalProcessed = 0;
        
        cout << "\n--- Processing Waiting Queues ---\n";
        cout << "Using 2:1 allocation ratio (2 critical : 1 normal)\n";
        cout << string(60, '-') << endl;
        
        while (true) {
            bool allocated = false;
            
            // Process 2 critical victims
            for (int i = 0; i < 2; i++) {
                if (!criticalQueue.isEmpty()) {
                    Victim v = criticalQueue.extractMax();
                    int nearestShelter = findNearestShelter(v.currentLocation, v.hasSpecialNeeds);
                    
                    if (nearestShelter != -1) {
                        shelters[nearestShelter].currentOccupants++;
                        
                        // Record allocation
                        if (allocatedCount < MAX_ALLOCATED) {
                            allocatedVictims[allocatedCount].victim = v;
                            allocatedVictims[allocatedCount].shelterIndex = nearestShelter;
                            allocatedVictims[allocatedCount].allocationTime = currentTime++;
                            allocatedVictims[allocatedCount].isActive = true;
                            allocatedCount++;
                        }
                        
                        int distance = findShortestDistance(v.currentLocation, 
                                                           shelters[nearestShelter].locationID);
                        
                        cout << "🚨 Critical: " << v.name 
                             << " (Severity: " << v.severity 
                             << ") → " << shelters[nearestShelter].name 
                             << " [" << distance << " km]" << endl;
                        criticalProcessed++;
                        allocated = true;
                    } else {
                        criticalQueue.insert(v);
                        if (allocated || !normalQueue.isEmpty()) {
                            cout << "⚠ No shelters available. Stopping.\n";
                        }
                        goto end_processing;
                    }
                }
            }
            
            // Process 1 normal victim
            if (!normalQueue.isEmpty()) {
                Victim v = normalQueue.dequeue();
                int nearestShelter = findNearestShelter(v.currentLocation, v.hasSpecialNeeds);
                
                if (nearestShelter != -1) {
                    shelters[nearestShelter].currentOccupants++;
                    
                    // Record allocation
                    if (allocatedCount < MAX_ALLOCATED) {
                        allocatedVictims[allocatedCount].victim = v;
                        allocatedVictims[allocatedCount].shelterIndex = nearestShelter;
                        allocatedVictims[allocatedCount].allocationTime = currentTime++;
                        allocatedVictims[allocatedCount].isActive = true;
                        allocatedCount++;
                    }
                    
                    int distance = findShortestDistance(v.currentLocation, 
                                                       shelters[nearestShelter].locationID);
                    
                    cout << "✓ Normal: " << v.name 
                         << " (Severity: " << v.severity 
                         << ") → " << shelters[nearestShelter].name 
                         << " [" << distance << " km]" << endl;
                    normalProcessed++;
                    allocated = true;
                } else {
                    normalQueue.enqueue(v);
                    if (allocated) {
                        cout << "⚠ No shelters available. Stopping.\n";
                    }
                    goto end_processing;
                }
            }
            
            if (criticalQueue.isEmpty() && normalQueue.isEmpty()) {
                break;
            }
            
            if (!allocated) {
                break;
            }
        }
        
        end_processing:
        
        int totalProcessed = criticalProcessed + normalProcessed;
        
        if (totalProcessed == 0) {
            cout << "⚠ No shelters available. All victims remain in queues.\n";
        } else {
            cout << string(60, '-') << endl;
            cout << "✓ Allocation Complete:\n";
            cout << "  • Critical victims allocated: " << criticalProcessed << endl;
            cout << "  • Normal victims allocated: " << normalProcessed << endl;
            cout << "  • Total processed: " << totalProcessed << endl;
            
            if (!criticalQueue.isEmpty() || !normalQueue.isEmpty()) {
                cout << "\n⚠ Still Waiting:\n";
                if (!criticalQueue.isEmpty()) {
                    cout << "  • Critical queue: " << criticalQueue.getSize() << " victims\n";
                }
                if (!normalQueue.isEmpty()) {
                    cout << "  • Normal queue: " << normalQueue.getSize() << " victims\n";
                }
            } else {
                cout << "\n🎉 All waiting victims have been allocated!\n";
            }
        }
        
        saveAllData();
    }
    
    // ==================== DISCHARGE VICTIM ====================
    void dischargeVictim() {
        string name;
        cout << "\n--- Discharge Victim ---\n";
        cout << "Enter victim name: ";
        cin.ignore();
        getline(cin, name);
        
        for (int i = 0; i < allocatedCount; i++) {
            if (allocatedVictims[i].isActive && 
                allocatedVictims[i].victim.name == name) {
                
                int shelterIdx = allocatedVictims[i].shelterIndex;
                shelters[shelterIdx].currentOccupants--;
                allocatedVictims[i].isActive = false;
                
                cout << "\n✓ Victim discharged successfully!\n";
                cout << "   Name: " << name << endl;
                cout << "   From: " << shelters[shelterIdx].name << endl;
                cout << "   New capacity: " << (shelters[shelterIdx].capacity - 
                                                shelters[shelterIdx].currentOccupants) << endl;
                
                saveAllData();
                
                // Auto-process queues if space available
                if (!criticalQueue.isEmpty() || !normalQueue.isEmpty()) {
                    cout << "\n🔄 Space available! Processing waiting queues...\n";
                    processWaitingQueues();
                }
                return;
            }
        }
        
        cout << "\n⚠ Victim not found in any shelter!\n";
    }
    
    // ==================== SEARCH VICTIM ====================
    void searchVictim() {
        string name;
        cout << "\n--- Search Victim ---\n";
        cout << "Enter victim name: ";
        cin.ignore();
        getline(cin, name);
        
        bool found = false;
        
        // Search in allocated
        for (int i = 0; i < allocatedCount; i++) {
            if (allocatedVictims[i].isActive && 
                allocatedVictims[i].victim.name == name) {
                
                cout << "\n✓ Found in Shelter:\n";
                cout << "   Name: " << allocatedVictims[i].victim.name << endl;
                cout << "   Shelter: " << shelters[allocatedVictims[i].shelterIndex].name << endl;
                cout << "   Severity: " << allocatedVictims[i].victim.severity << endl;
                cout << "   Contact: " << allocatedVictims[i].victim.contactNumber << endl;
                if (allocatedVictims[i].victim.hasSpecialNeeds)
                    cout << "   Special Needs: " << allocatedVictims[i].victim.specialNeeds << endl;
                found = true;
                break;
            }
        }
        
        if (!found) {
            found = criticalQueue.searchVictim(name);
        }
        
        if (!found) {
            found = normalQueue.searchVictim(name);
        }
        
        if (!found) {
            cout << "\n⚠ Victim not found in system!\n";
        }
    }
    
    // ==================== EXPAND SHELTER ====================
    void expandShelterCapacity() {
        displayShelters();
        
        int shelterChoice;
        cout << "\nEnter shelter number to expand (1-" << shelterCount << "): ";
        cin >> shelterChoice;
        shelterChoice--;
        
        if (shelterChoice < 0 || shelterChoice >= shelterCount) {
            cout << "Invalid shelter selection!\n";
            return;
        }
        
        int additionalCapacity;
        cout << "Enter additional capacity: ";
        cin >> additionalCapacity;
        
        if (additionalCapacity <= 0) {
            cout << "Invalid capacity!\n";
            return;
        }
        
        shelters[shelterChoice].capacity += additionalCapacity;
        
        cout << "\n✓ Shelter capacity expanded!\n";
        cout << "   Shelter: " << shelters[shelterChoice].name << endl;
        cout << "   New capacity: " << shelters[shelterChoice].capacity << endl;
        cout << "   Available space: " << (shelters[shelterChoice].capacity - 
                                           shelters[shelterChoice].currentOccupants) << endl;
        
        saveAllData();
        
        if (!criticalQueue.isEmpty() || !normalQueue.isEmpty()) {
            cout << "\n🔄 Processing waiting queues with expanded capacity...\n";
            processWaitingQueues();
        }
    }
    
    // ==================== DISPLAY FUNCTIONS ====================
    void displayShelters() {
        if (shelterCount == 0) {
            cout << "\n⚠ No shelters in system!\n";
            return;
        }
        
        cout << "\n========== SHELTER STATUS ==========\n";
        cout << left << setw(4) << "No." << setw(25) << "Shelter Name" 
             << setw(10) << "Capacity" << setw(10) << "Occupied" 
             << setw(10) << "Available" << setw(10) << "Location" << "Contact\n";
        cout << string(90, '=') << endl;
        
        for (int i = 0; i < shelterCount; i++) {
            int available = shelters[i].capacity - shelters[i].currentOccupants;
            cout << left << setw(4) << (i+1)
                 << setw(25) << shelters[i].name
                 << setw(10) << shelters[i].capacity
                 << setw(10) << shelters[i].currentOccupants
                 << setw(10) << available
                 << setw(10) << shelters[i].locationID
                 << shelters[i].contactNumber << endl;
        }
    }
    
    void displayWaitingQueues() {
        cout << "\n========== WAITING QUEUES ==========\n";
        criticalQueue.display();
        cout << endl;
        normalQueue.display();
    }
    
    void displayStatistics() {
        int totalCapacity = 0, totalOccupied = 0;
        int activeAllocated = 0;
        
        for (int i = 0; i < shelterCount; i++) {
            totalCapacity += shelters[i].capacity;
            totalOccupied += shelters[i].currentOccupants;
        }
        
        for (int i = 0; i < allocatedCount; i++) {
            if (allocatedVictims[i].isActive) activeAllocated++;
        }
        
        cout << "\n========== SYSTEM STATISTICS ==========\n";
        cout << "Total Locations: " << locationCount << endl;
        cout << "Total Shelters: " << shelterCount << endl;
        cout << "Total Capacity: " << totalCapacity << endl;
        cout << "Total Occupied: " << totalOccupied << endl;
        cout << "Available Space: " << (totalCapacity - totalOccupied) << endl;
        cout << "Occupancy Rate: " << fixed << setprecision(1) 
             << (totalCapacity > 0 ? (totalOccupied * 100.0 / totalCapacity) : 0) << "%\n";
        cout << "\nQueue Status:\n";
        cout << "  Critical Queue: " << criticalQueue.getSize() << " victims\n";
        cout << "  Normal Queue: " << normalQueue.getSize() << " victims\n";
        cout << "  Total Waiting: " << (criticalQueue.getSize() + normalQueue.getSize()) << "\n";
        cout << "\nAllocation History:\n";
        cout << "  Total Allocated: " << activeAllocated << endl;
        cout << "  Total Discharged: " << (allocatedCount - activeAllocated) << endl;
        cout << "======================================\n";
    }
    
    void generateDetailedReport() {
        cout << "\n╔════════════════════════════════════════╗\n";
        cout << "║      DETAILED SYSTEM REPORT           ║\n";
        cout << "╚════════════════════════════════════════╝\n";
        
        displayStatistics();
        cout << "\n";
        displayShelters();
        cout << "\n";
        displayWaitingQueues();
        
        cout << "\n========== CURRENTLY SHELTERED ==========\n";
        bool anyActive = false;
        for (int i = 0; i < allocatedCount; i++) {
            if (allocatedVictims[i].isActive) {
                if (!anyActive) {
                    cout << left << setw(20) << "Name" << setw(25) << "Shelter" 
                         << setw(10) << "Severity" << "Contact\n";
                    cout << string(75, '-') << endl;
                    anyActive = true;
                }
                cout << left << setw(20) << allocatedVictims[i].victim.name
                     << setw(25) << shelters[allocatedVictims[i].shelterIndex].name
                     << setw(10) << allocatedVictims[i].victim.severity
                     << allocatedVictims[i].victim.contactNumber << endl;
            }
        }
        if (!anyActive) {
            cout << "No victims currently in shelters.\n";
        }
    }
    
    // ==================== ADD NEW SHELTER ====================
    void addNewShelter() {
        if (locationCount == 0) {
            cout << "\n⚠ Error: Graph not configured! Please setup graph first (Option 15).\n";
            return;
        }
        
        if (shelterCount >= MAX_SHELTERS) {
            cout << "\n⚠ Maximum shelter limit reached!\n";
            return;
        }
        
        string name, contact;
        int capacity, location;
        
        cout << "\n--- Add New Shelter ---\n";
        cout << "Enter shelter name: ";
        cin.ignore();
        getline(cin, name);
        
        cout << "Enter contact number: ";
        getline(cin, contact);
        
        cout << "Enter shelter capacity: ";
        cin >> capacity;
        
        while (capacity <= 0) {
            cout << "Invalid! Enter positive capacity: ";
            cin >> capacity;
        }
        
        cout << "Enter shelter location (0-" << locationCount-1 << "): ";
        cin >> location;
        
        while (location < 0 || location >= locationCount) {
            cout << "Invalid! Enter location between 0-" << locationCount-1 << ": ";
            cin >> location;
        }
        
        addShelter(name, capacity, location, contact);
        
        cout << "\n✓ Shelter added successfully!\n";
        cout << "   Name: " << name << endl;
        cout << "   Capacity: " << capacity << endl;
        cout << "   Location: " << location << endl;
        
        saveAllData();
        
        if (!criticalQueue.isEmpty() || !normalQueue.isEmpty()) {
            cout << "\n🔄 New shelter available! Processing waiting queues...\n";
            processWaitingQueues();
        }
    }
    
    // ==================== PRIORITY OVERRIDE ====================
    void priorityOverride() {
        cout << "\n--- Emergency Priority Override ---\n";
        cout << "This allows immediate allocation bypassing normal queue.\n";
        
        if (normalQueue.isEmpty()) {
            cout << "⚠ Normal queue is empty!\n";
            return;
        }
        
        Victim v = normalQueue.peek();
        cout << "Next in normal queue: " << v.name << " (Severity: " << v.severity << ")\n";
        cout << "Override and allocate immediately? (1=Yes, 0=No): ";
        
        int choice;
        cin >> choice;
        
        if (choice != 1) {
            cout << "Override cancelled.\n";
            return;
        }
        
        v = normalQueue.dequeue();
        int nearestShelter = findNearestShelter(v.currentLocation, v.hasSpecialNeeds);
        
        if (nearestShelter != -1) {
            shelters[nearestShelter].currentOccupants++;
            
            if (allocatedCount < MAX_ALLOCATED) {
                allocatedVictims[allocatedCount].victim = v;
                allocatedVictims[allocatedCount].shelterIndex = nearestShelter;
                allocatedVictims[allocatedCount].allocationTime = currentTime++;
                allocatedVictims[allocatedCount].isActive = true;
                allocatedCount++;
            }
            
            int distance = findShortestDistance(v.currentLocation, 
                                               shelters[nearestShelter].locationID);
            
            cout << "\n🚨 PRIORITY OVERRIDE SUCCESSFUL\n";
            cout << "   Victim: " << v.name << endl;
            cout << "   Shelter: " << shelters[nearestShelter].name << endl;
            cout << "   Distance: " << distance << " km\n";
            
            saveAllData();
        } else {
            normalQueue.enqueue(v);
            cout << "\n⚠ No shelters available. Victim returned to queue.\n";
        }
    }
    
    // ==================== TOGGLE SHELTER STATUS ====================
    void toggleShelterStatus() {
        displayShelters();
        
        int shelterChoice;
        cout << "\nEnter shelter number to activate/deactivate (1-" << shelterCount << "): ";
        cin >> shelterChoice;
        shelterChoice--;
        
        if (shelterChoice < 0 || shelterChoice >= shelterCount) {
            cout << "Invalid shelter selection!\n";
            return;
        }
        
        shelters[shelterChoice].isActive = !shelters[shelterChoice].isActive;
        
        cout << "\n✓ Shelter status changed!\n";
        cout << "   Shelter: " << shelters[shelterChoice].name << endl;
        cout << "   Status: " << (shelters[shelterChoice].isActive ? "ACTIVE" : "INACTIVE") << endl;
        
        if (!shelters[shelterChoice].isActive && shelters[shelterChoice].currentOccupants > 0) {
            cout << "\n⚠ WARNING: Shelter has " << shelters[shelterChoice].currentOccupants 
                 << " occupants!\n";
            cout << "   Please relocate them before deactivating.\n";
        }
        
        saveAllData();
    }
    
    // ==================== VIEW NEXT IN QUEUE ====================
    void viewNextInQueue() {
        cout << "\n========== NEXT IN QUEUES ==========\n";
        
        if (!criticalQueue.isEmpty()) {
            Victim v = criticalQueue.peek();
            cout << "\n🚨 Next Critical Victim:\n";
            cout << "   Name: " << v.name << endl;
            cout << "   Severity: " << v.severity << endl;
            cout << "   Location: " << v.currentLocation << endl;
            cout << "   Contact: " << v.contactNumber << endl;
            if (v.hasSpecialNeeds)
                cout << "   Special Needs: " << v.specialNeeds << endl;
        } else {
            cout << "\n🚨 Critical Queue: Empty\n";
        }
        
        if (!normalQueue.isEmpty()) {
            Victim v = normalQueue.peek();
            cout << "\n✓ Next Normal Victim:\n";
            cout << "   Name: " << v.name << endl;
            cout << "   Severity: " << v.severity << endl;
            cout << "   Location: " << v.currentLocation << endl;
            cout << "   Contact: " << v.contactNumber << endl;
            if (v.hasSpecialNeeds)
                cout << "   Special Needs: " << v.specialNeeds << endl;
        } else {
            cout << "\n✓ Normal Queue: Empty\n";
        }
    }
};

// ==================== MAIN FUNCTION ====================
int main() {
    DisasterShelterSystem system;
    system.setupSystem();
    
    int choice;
    
    do {
        cout << "\n╔════════════════════════════════════════╗\n";
        cout << "║  DISASTER SHELTER ALLOCATION SYSTEM   ║\n";
        cout << "╚════════════════════════════════════════╝\n";
        cout << " 1. Register Victim\n";
        cout << " 2. Process Waiting Queues\n";
        cout << " 3. Discharge Victim from Shelter\n";
        cout << " 4. Search Victim\n";
        cout << " 5. View Next in Queue\n";
        cout << " 6. Add New Shelter\n";
        cout << " 7. Expand Shelter Capacity\n";
        cout << " 8. Toggle Shelter Active/Inactive\n";
        cout << " 9. Priority Override (Emergency)\n";
        cout << "10. View Shelter Status\n";
        cout << "11. View Waiting Queues\n";
        cout << "12. View System Statistics\n";
        cout << "13. Generate Detailed Report\n";
        cout << "14. Save All Data Manually\n";
        cout << "15. Setup/Modify City Graph\n";
        cout << " 0. Exit\n";
        cout << "────────────────────────────────────────\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        switch(choice) {
            case 1:
                system.registerVictim();
                break;
            case 2:
                system.processWaitingQueues();
                break;
            case 3:
                system.dischargeVictim();
                break;
            case 4:
                system.searchVictim();
                break;
            case 5:
                system.viewNextInQueue();
                break;
            case 6:
                system.addNewShelter();
                break;
            case 7:
                system.expandShelterCapacity();
                break;
            case 8:
                system.toggleShelterStatus();
                break;
            case 9:
                system.priorityOverride();
                break;
            case 10:
                system.displayShelters();
                break;
            case 11:
                system.displayWaitingQueues();
                break;
            case 12:
                system.displayStatistics();
                break;
            case 13:
                system.generateDetailedReport();
                break;
            case 14:
                system.saveAllData();
                cout << "✓ Manual save complete!\n";
                break;
            case 15:
                system.setupGraph();
                break;
            case 0:
                system.saveAllData();
                cout << "\n╔════════════════════════════════════════╗\n";
                cout << "║   Thank you for using the system!     ║\n";
                cout << "║         Stay safe! 🏥                  ║\n";
                cout << "╚════════════════════════════════════════╝\n";
                break;
            default:
                cout << "\n⚠ Invalid choice! Please try again.\n";
        }
        
    } while(choice != 0);
    
    return 0;
}