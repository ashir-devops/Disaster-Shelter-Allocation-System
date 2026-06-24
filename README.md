# Disaster Shelter Allocation System

A C++ console-based Disaster Shelter Allocation System designed to efficiently allocate disaster victims to the nearest available shelters during emergency situations. The system prioritizes critical victims, manages shelter capacities, and uses graph-based shortest path calculations to optimize shelter assignments.

## Features

### Victim Management

* Register disaster victims
* Assign severity levels (1–10)
* Support for special needs victims
* Search victim records
* Discharge victims from shelters

### Shelter Management

* Add new shelters
* Expand shelter capacity
* Activate/Deactivate shelters
* Monitor shelter occupancy
* View shelter status and availability

### Priority-Based Allocation

* Critical victims (severity ≥ 7) stored in a priority queue (Max Heap)
* Normal victims stored in a Circular Queue
* 2:1 allocation ratio:

  * Two critical victims processed first
  * One normal victim processed afterward

### Route Optimization

* Uses Dijkstra's Algorithm to find the nearest available shelter
* Minimizes travel distance during emergency evacuations
* Supports multiple city locations connected through a weighted graph

### Data Persistence

System data is automatically stored using files:

* shelters.dat
* critical_queue.dat
* normal_queue.dat
* allocated.dat
* system.dat

All data remains available after restarting the application.

## Data Structures Used

| Data Structure | Purpose                            |
| -------------- | ---------------------------------- |
| Max Heap       | Critical victim priority queue     |
| Circular Queue | Normal victim waiting queue        |
| Arrays         | Shelter, victim, and graph storage |
| Weighted Graph | City location network              |

## Algorithms Implemented

### Dijkstra's Algorithm

Used to determine the shortest distance between a victim's location and available shelters.

### Heap-Based Priority Scheduling

Critical victims receive higher priority based on severity level.

### Queue-Based Processing

Normal victims are processed in First-In-First-Out (FIFO) order.

## System Modules

### Victim Registration

* Register victims
* Capture contact information
* Store severity levels
* Record special needs

### Shelter Allocation

* Automatic nearest-shelter assignment
* Capacity checking
* Occupancy tracking

### Queue Management

* Critical queue monitoring
* Normal queue monitoring
* Emergency priority override

### Reporting

* Shelter status reports
* Waiting queue reports
* System statistics
* Detailed allocation reports

## Main Menu Functions

1. Register Victim
2. Process Waiting Queues
3. Discharge Victim
4. Search Victim
5. View Next in Queue
6. Add New Shelter
7. Expand Shelter Capacity
8. Toggle Shelter Status
9. Emergency Priority Override
10. View Shelter Status
11. View Waiting Queues
12. View System Statistics
13. Generate Detailed Report
14. Save All Data
15. Setup/Modify City Graph

## Technologies Used

* C++
* Object-Oriented Programming
* File Handling
* Graph Algorithms
* Heap Data Structure
* Queue Data Structure
* Standard Template Libraries

## Learning Outcomes

This project demonstrates:

* Disaster Management System Design
* Priority-Based Resource Allocation
* Graph Theory Applications
* Dijkstra's Shortest Path Algorithm
* Heap and Queue Implementation
* File-Based Data Persistence
* Real-World Emergency Response Simulation

## How to Run

### Compile

```bash
g++ main.cpp -o DisasterShelterSystem
```

### Run

```bash
./DisasterShelterSystem
```

For Windows:

```bash
g++ main.cpp -o DisasterShelterSystem.exe
DisasterShelterSystem.exe
```

## Future Enhancements

* GUI Version
* Database Integration (MySQL/MongoDB)
* GIS/Map Integration
* Real-Time Emergency Notifications
* Multi-City Support
* Web-Based Dashboard

## Academic Project

Developed as a Data Structures & Algorithms / Object-Oriented Programming project to demonstrate efficient disaster response management through algorithmic decision-making and resource optimization.
