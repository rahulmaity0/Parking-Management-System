#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>

using namespace std;

class Vehicle {
private:
    string licensePlate;
    string vehicleType;
    time_t entryTime;
    bool isParked;

public:
    Vehicle(string plate, string type) : 
        licensePlate(plate), vehicleType(type), isParked(false) {}

    string getLicensePlate() const { return licensePlate; }
    string getVehicleType() const { return vehicleType; }
    time_t getEntryTime() const { return entryTime; }
    bool getIsParked() const { return isParked; }

    void parkVehicle() {
        entryTime = time(nullptr);
        isParked = true;
    }

    void exitVehicle() {
        isParked = false;
    }
};

class ParkingSpot {
private:
    int spotNumber;
    bool isOccupied;
    string vehicleType;

public:
    ParkingSpot(int number, string type) : 
        spotNumber(number), isOccupied(false), vehicleType(type) {}

    int getSpotNumber() const { return spotNumber; }
    bool getIsOccupied() const { return isOccupied; }
    string getVehicleType() const { return vehicleType; }

    void occupySpot() { isOccupied = true; }
    void vacateSpot() { isOccupied = false; }
};

class ParkingLot {
private:
    vector<ParkingSpot> spots;
    vector<Vehicle> parkedVehicles;
    const double HOURLY_RATE = 2.50;
    const int MAX_SPOTS = 50;

    void saveToFile() {
        ofstream file("parking_data.txt");
        if (file.is_open()) {
            for (const auto& vehicle : parkedVehicles) {
                if (vehicle.getIsParked()) {
                    file << vehicle.getLicensePlate() << ","
                         << vehicle.getVehicleType() << ","
                         << vehicle.getEntryTime() << endl;
                }
            }
            file.close();
        }
    }

public:
    ParkingLot() {
        // Initialize parking spots
        for (int i = 1; i <= MAX_SPOTS; i++) {
            string type = (i <= 30) ? "Car" : "Motorcycle";
            spots.push_back(ParkingSpot(i, type));
        }
        loadFromFile();
    }

    void loadFromFile() {
        ifstream file("parking_data.txt");
        string line;
        
        while (getline(file, line)) {
            size_t pos = 0;
            string token;
            vector<string> tokens;
            
            while ((pos = line.find(",")) != string::npos) {
                token = line.substr(0, pos);
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }
            tokens.push_back(line);

            if (tokens.size() == 3) {
                Vehicle vehicle(tokens[0], tokens[1]);
                parkedVehicles.push_back(vehicle);
            }
        }
        file.close();
    }

    int findAvailableSpot(const string& vehicleType) {
        for (const auto& spot : spots) {
            if (!spot.getIsOccupied() && spot.getVehicleType() == vehicleType) {
                return spot.getSpotNumber();
            }
        }
        return -1;
    }

    bool parkVehicle(const string& licensePlate, const string& vehicleType) {
        int spotNumber = findAvailableSpot(vehicleType);
        if (spotNumber == -1) {
            return false;
        }

        Vehicle vehicle(licensePlate, vehicleType);
        vehicle.parkVehicle();
        parkedVehicles.push_back(vehicle);
        spots[spotNumber - 1].occupySpot();
        saveToFile();
        return true;
    }

    double calculateCharges(time_t entryTime) {
        time_t exitTime = time(nullptr);
        double hours = difftime(exitTime, entryTime) / 3600.0;
        return hours * HOURLY_RATE;
    }

    bool removeVehicle(const string& licensePlate) {
        for (auto& vehicle : parkedVehicles) {
            if (vehicle.getLicensePlate() == licensePlate && vehicle.getIsParked()) {
                double charges = calculateCharges(vehicle.getEntryTime());
                cout << "Parking charges: $" << fixed << setprecision(2) << charges << endl;
                
                vehicle.exitVehicle();
                for (auto& spot : spots) {
                    if (spot.getVehicleType() == vehicle.getVehicleType() && 
                        spot.getIsOccupied()) {
                        spot.vacateSpot();
                        break;
                    }
                }
                saveToFile();
                return true;
            }
        }
        return false;
    }

    void displayStatus() {
        cout << "\nParking Lot Status:\n";
        cout << "Total spots: " << MAX_SPOTS << "\n";
        int occupiedCar = 0, occupiedMoto = 0;
        
        for (const auto& spot : spots) {
            if (spot.getIsOccupied()) {
                if (spot.getVehicleType() == "Car") occupiedCar++;
                else occupiedMoto++;
            }
        }
        
        cout << "Occupied car spots: " << occupiedCar << "/30\n";
        cout << "Occupied motorcycle spots: " << occupiedMoto << "/20\n";
        
        cout << "\nCurrently parked vehicles:\n";
        for (const auto& vehicle : parkedVehicles) {
            if (vehicle.getIsParked()) {
                time_t entryTime = vehicle.getEntryTime();
                cout << "License Plate: " << vehicle.getLicensePlate()
                     << " | Type: " << vehicle.getVehicleType()
                     << " | Entry Time: " << ctime(&entryTime);
            }
        }
    }
};

int main() {
    ParkingLot parkingLot;
    string input, licensePlate, vehicleType;
    
    while (true) {
        cout << "\nParking Management System\n";
        cout << "1. Park Vehicle\n";
        cout << "2. Remove Vehicle\n";
        cout << "3. Display Status\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        getline(cin, input);

        if (input == "1") {
            cout << "Enter license plate: ";
            getline(cin, licensePlate);
            cout << "Enter vehicle type (Car/Motorcycle): ";
            getline(cin, vehicleType);
            
            if (parkingLot.parkVehicle(licensePlate, vehicleType)) {
                cout << "Vehicle parked successfully!\n";
            } else {
                cout << "No available spots for " << vehicleType << "!\n";
            }
        }
        else if (input == "2") {
            cout << "Enter license plate: ";
            getline(cin, licensePlate);
            
            if (parkingLot.removeVehicle(licensePlate)) {
                cout << "Vehicle removed successfully!\n";
            } else {
                cout << "Vehicle not found!\n";
            }
        }
        else if (input == "3") {
            parkingLot.displayStatus();
        }
        else if (input == "4") {
            break;
        }
        else {
            cout << "Invalid choice!\n";
        }
    }
    
    return 0;
}