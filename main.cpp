#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map> /*= eine Hashmap Such/Einfüg/Löschoperationen O(1) https://www.geeksforgeeks.org/unordered_map-in-cpp-stl/*/
#include <set> //zum Ausgeben nach Linie
#include <queue> //priority queue
#include <limits>
#include <stdexcept>
#include <algorithm> //reverse



using namespace std;

// Struktur für gewichtete Kante mit Linieninfo
struct KantenInfo {
    string to; //name der benachbarten Station
    int weight; //Zeit bis zur nächsten Station
    string line; //Linie (z.B. U1 oder 5) mit der die benachbarte Station erreicht wird
};

struct KnotenInfo {
    bool visited;
    int total_weight;// Kürzeste bekannte Distanz
    string vorgaengerKnoten; //über welchen Knoten bin ich zu diesem Knoten gekommen
    string linieZuVorgaenger; //von welcher Linie komme ich
};

struct HeapElement {
    string station;
    int total_weight;

    // Für Min-Heap (kleinste Gewicht zuerst)
    bool operator>(const HeapElement& other) const {
        return total_weight > other.total_weight;
    }
};



//FUNKTIONS DEKLARATIONEN
string trim(const string& str); //zum Entfernen führender/folgender Leerzeichen
void printGraphByLine(const unordered_map<string, vector<KantenInfo>>& graph);
void printNeighborsStation(const vector<KantenInfo>& neighbors);

void dijkstra(unordered_map<string, vector<KantenInfo>> graph, string startknoten, string endknoten);
void printPath(unordered_map<string, KnotenInfo> node_data,string startknoten, string endknoten);




int main(int argc, char* argv[]) { //argv[1] 0 relativer Pfad zu Graph | argv[2] = StartStation | argv[3] = ZielStation
////////////////////////////////////////////////////////////
//            Einlesen des Graphen aus der Datei          //
////////////////////////////////////////////////////////////
    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Fehler beim Öffnen der Datei!" << endl;
        return 1;
    }


    unordered_map<string, vector<KantenInfo>> graph;
    string line;

    while (getline(file, line)) {
        istringstream iss(line); //*i*stringstream ist im gegensatz zum stringstream nur für Input (wir können nichts dazu schreiben)

        string lineName;
        getline(iss, lineName, ':'); // "Lies aus iss (also der Zeile in dem File) solange in den String lineName, bis du : findest"
        lineName = trim(lineName); //Entfernt führende und folgende "Whitespaces" <- definiert in der Funktion

        string prevStation, currStation;
        int weight;
        string dummy; //verhindert, dass die " eingelesen werden

        //erste Station der Linie
        iss >> ws; //std::ws (steht für whitespace) in kombination mit >> extrahiert alle führenden whitespaces
        getline(iss, dummy, '"'); //' lies aus iss solange in dummy bis du ein " findest '

        if (!getline(iss, prevStation, '"')) continue; //versuche in prevStation bis zumnächsten " einzulesen, wenn das nicht klappt (e.g. du findest kein "), dann überspringe diesen Loop -> gehe zur nächsten Zeile im File über

        //alle weiteren Stationen der Linie
        while (iss >> weight) { //jetzt wird das gewicht eingelesen
            iss >> ws;
            getline(iss, dummy, '"');
            if (!getline(iss, currStation, '"')) break;

            // Hinzufügen Kante/EdgeInfo -BEIDE- Richtungen
            graph[prevStation].push_back({currStation, weight, lineName});
            graph[currStation].push_back({prevStation, weight, lineName});

            prevStation = currStation;
        }
    }

    file.close();


//Tests:
//printGraphByLine(graph);
//printNeighborsStation(graph["Matzleinsdorferplatz"]);


///////////////////////////////////////////////////
//              Finden einer Route               //
///////////////////////////////////////////////////


    string startknoten = argv[2];
    string endknoten= argv[3];

    cout <<"start: " << startknoten << " Ende: " << endknoten << endl;

    try {
        dijkstra (graph, startknoten, endknoten);
    } catch(logic_error const& e) {
        cout << "Logic Error: "<< e.what()<<endl;
    }
    return 0;
}



//FUNKTIONSDEFINITIONEN

string trim(const string& str) {
    const char* whitespace = " \t\n\r"; //gibt an Welche Zeichen als "whitespace" gelten
    size_t start = str.find_first_not_of(whitespace); //sucht das Erste Zeichen das kein "Whitespace" ist
    if (start == string::npos) return ""; //string::npos = "bis zum Ende des Strings"
    size_t end = str.find_last_not_of(whitespace); // posititon des letzten Zeichens das kein "whitespace" ist.
    return str.substr(start, end - start + 1); //wir geben nur den String zwischen start und end aus. (aber start soll ja dabei sein, deshalb end-start+1
}



//DIJKSTRA ALGORITHMUS ERKLÄRT https://www.youtube.com/watch?v=KiOso3VE-vI
//https://mmf.univie.ac.at/fileadmin/user_upload/p_mathematikmachtfreunde/Materialien/AB-Dijkstra-Algorithmus-Ausarbeitung.pdf
//PRIORITY QUEUE -> https://en.cppreference.com/w/cpp/container/priority_queue (push, pop)
/*
Zwei Mengen
Besucht B -> kürzeste Distanz bekannt
Neu N (nicht besucht)

Am Anfang haben alle Knoten Distanz unendlich.
Von einem B Knoten werden immer die Distanz dorthin mit der eingetragenen Distanz verglichen.
Ist die neue Distanz kleiner wie die eingetragene Distanz wird sie ersetzt und der neue Vorgänger eingetragen.
Sobald ich einen Knoten besuche kenne ich die kürzeste Distanz dorthin
Sobald ein Knoten aus dem Heap geholt wird, ist seine Distanz endgültig!!!!
Im Heap können mehrere Einträge der gleichen Station stehen, da die Einträge nicht ersetzt werden sondern nur neue hinzugefügt
veraltete Einträge überspringen!
*/

void dijkstra(unordered_map<string, vector<KantenInfo>> graph, string startknoten, string endknoten) {

//Initalisieren
    unordered_map<string, KnotenInfo> node_data;
    priority_queue<HeapElement, vector<HeapElement>, greater<HeapElement>> heap;

//Knoten als unbesucht markieren & Distanz auf "unendlich" setzen & vorgänger leer
//for (const std::pair<const std::string, std::vector<KantenInfo>>& entry : graph) {
    for (const auto& entry : graph) {
        node_data[entry.first].visited = false;
        node_data[entry.first].total_weight = numeric_limits<int>::max();
        node_data[entry.first].vorgaengerKnoten = "";
        node_data[entry.first].linieZuVorgaenger = "";
    }

    node_data[startknoten].total_weight = 0;
    heap.push({startknoten, 0});

    string currentStation = startknoten;
    int currentWeight;
    bool zielKnotenErreicht = false;

    //START Algorithmus
    int counter = 1;

    do {
        cout << "\nAnzahl besuchter Stationen: " << counter<<endl;

        currentStation = heap.top().station; //die station mit dem geringsten Gewicht
        currentWeight = heap.top().total_weight;
        heap.pop();


        if (currentWeight > node_data[currentStation].total_weight) { //veraltete Station im heap
            cout << "Veralteter Eintrag fuer " << currentStation << endl;
            continue;
        }

         cout <<"Aktuelle Station: "<<currentStation<<
        "| fixe Kosten: " << node_data[currentStation].total_weight<<endl;

        if (currentStation == endknoten) {
            zielKnotenErreicht = true;
            break;
        }
        node_data[currentStation].visited = true;



        /*for each (neighbour m of i){
        if (not visited[m]) {
        // Nachbarknoten in Heap einordnen
        Heap.put(m, m.gewicht + minweg);
        }}
        */
        for (const KantenInfo& kante : graph[currentStation]) {
            const string& nachbar = kante.to;

            if (!node_data[nachbar].visited) {

                int new_weight = currentWeight + kante.weight; //"Was würde es kosten, von Start bis zu nachbar, über currentStation zu gehen?"
                if (new_weight < node_data[nachbar].total_weight) { //"Wenn der neue Weg über currentStation kürzer ist als der bisher bekannte Weg zum Knoten nachbar, dann aktualisiere diesen Weg."
                    node_data[nachbar].total_weight = new_weight;
                    node_data[nachbar].vorgaengerKnoten = currentStation;
                    node_data[nachbar].linieZuVorgaenger = kante.line;
                    heap.push({nachbar, new_weight});

                    cout << "Nachbar " << nachbar << " unbesucht " <<
                    "| aktuelle Kosten : " << node_data[nachbar].total_weight<<endl;
                }
            } else {
                cout << "Nachbar " << nachbar << " bereits besucht " <<
                "| fixe Kosten: " << node_data[nachbar].total_weight<<endl;
            }
        }

        //alle Auswahlmöglichkeiten für den nächsten besuchbaren Knoten sind im Heap
        //durch priority queue ist der Knoten mit dem niedrigsten gewicht vorne
        //diesen besuche ich im nächsten durchlauf der Schleife -> nehme ihn aus dem Heap raus

        counter++;

    } while (!zielKnotenErreicht && !heap.empty());


    //!heap.empty() -> Station nicht erreichbar / nicht vorhanden (vertippt) Abbruch der Schleife, wenn alle Stationen besucht wurden
    //oder abbruch der Schleife wenn das Ziel erreicht wurde


    if (zielKnotenErreicht) {
        printPath(node_data,startknoten, endknoten);
    } else {
        cout << "\nKein Weg von \"" << startknoten
             << "\" zu \"" << endknoten << "\" gefunden!" << endl;
    }
}

void printPath(unordered_map<string, KnotenInfo> node_data,string startknoten, string endknoten) {

    if (node_data[endknoten].total_weight == numeric_limits<int>::max()) {
        throw logic_error("Kein Pfad zum Zielknoten gefunden!");
    }

    vector<pair<string, string>> pfadSchritte; // {Station, Linie}
    string aktuell = endknoten;

    // Pfad rückwärts sammeln
    while (aktuell != startknoten) {
        pfadSchritte.push_back({aktuell, node_data[aktuell].linieZuVorgaenger});
        aktuell = node_data[aktuell].vorgaengerKnoten;
    }

    reverse(pfadSchritte.begin(), pfadSchritte.end()); //Algorithmus aus <algorithm>

    // Ausgabe des Pfads
    cout << "\n--- Kuerzester Pfad (" << node_data[endknoten].total_weight << " Minuten) ---\n";
    cout << "[START] " << startknoten << endl;

    string letzte_linie = "";
    for (const auto& schritt : pfadSchritte) {
        string station = schritt.first;
        string linie = schritt.second;

        if (letzte_linie != linie) {
            if (!letzte_linie.empty()) {
                cout << " (Umstieg zu " << linie << ")"<<endl;
            }
            cout << "[" << linie << "] ";
        } else {
            cout << " --> ";
        }

        cout << station;
        letzte_linie = linie;
    }

    cout << "\n[ZIEL] " << endknoten << "\n" << endl;
}




//TEST
void printGraphByLine(const unordered_map<string, vector<KantenInfo>>& graph) {
    // Gruppierung nach Linien zum Ausgabezweck
    unordered_map<string, set<pair<string, string>>> linienGraph;

    for (const auto& entry : graph) {
        const string& from = entry.first;
        for (const KantenInfo& edge : entry.second) {
            // alphabetisch sortieren, um Duplikate zu vermeiden
            string s1 = from, s2 = edge.to;
            if (s1 > s2) swap(s1, s2);
            linienGraph[edge.line].insert({s1, s2});
        }
    }

    // Ausgabe nach Linien gruppiert
    for (const auto& entry : linienGraph) {
        const string& linie = entry.first;
        cout << linie << ":\n";
        for (const auto& conn : entry.second) {
            const string& s1 = conn.first;
            const string& s2 = conn.second;
            int weight = -1;

            // Suche Gewicht in graph[s1]
            const auto& edges = graph.at(s1);
            for (const auto& edge : edges) {
                if (edge.to == s2 && edge.line == linie) {
                    weight = edge.weight;
                    break;
                }
            }

            cout << "  " << s1 << " <-" << weight << "-> " << s2 << endl;
        }
        cout << endl;
    }
}

void printNeighborsStation(const vector<KantenInfo>& neighbors) {
    if (neighbors.empty()) {
        cout << "Keine Nachbarstationen gefunden.\n";
        return;
    }

    cout << "Nachbarstationen:\n";
    for (const KantenInfo& edge : neighbors) {
        cout << "  -> " << edge.to
             << " (Linie: " << edge.line
             << ", Gewicht: " << edge.weight << ")\n";
    }


}
