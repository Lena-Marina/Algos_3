/* TO-Dos:
Lena:
-)  Kommandozeilen Parameter ergänzen (1. Parameter-> relativer Pfad zu file. ; 2. Parameter-> Startstation ; 3. Parameter -> Zielstation
-)  Protokoll fürs Einlesen schreiben


Jana:




*/


#include <iostream>
#include <fstream>
#include <sstream> /**/
#include <string>
#include <vector>
#include <unordered_map> /*= eine Hashmap Such/Einfüg/Löschoperationen O(1) https://www.geeksforgeeks.org/unordered_map-in-cpp-stl/*/
#include <set> //zum Ausgeben nach Linie



using namespace std;

// Struktur für gewichtete Kante mit Linieninfo
struct EdgeInfo
{
    string to; //name der benachbarten Station
    int weight; //Zeit bis zur nächsten Station
    string line; //Linie (z.B. U1 oder 5) mit der die benachbarte Station erreicht wird
};

//FUNKTIONS DEKLARATIONEN
string trim(const string& str); //zum Entfernen führender/folgender Leerzeichen
void printGraphByLine(const unordered_map<string, vector<EdgeInfo>>& graph);
void printNeighborsStation(const vector<EdgeInfo>& neighbors);




int main()
{
////////////////////////////////////////////////////////////
//            Einlesen des Graphen aus der Datei          //
////////////////////////////////////////////////////////////
    ifstream file("stationen.txt");
    if (!file.is_open())
    {
        cerr << "Fehler beim Öffnen der Datei!" << endl;
        return 1;
    }


    unordered_map<string, vector<EdgeInfo>> graph;
    string line;

    while (getline(file, line))
    {
        istringstream iss(line); //*i*stringstream ist im gegensatz zum stringstream nur für Input (wir können nichts dazu schreiben)

        string lineName;
        getline(iss, lineName, ':'); // "Lies aus iss (also der Zeile in dem File) solange in den String lineName, bis du : findest"
        lineName = trim(lineName); //Entfernt führende und folgende "Whitespaces" <- definiert in der Funktion

        string prevStation, currStation;
        int weight;
        string dummy; //verhindert, dass die " eingelesen werden


        iss >> ws; //std::ws (steht für whitespace) in kombination mit >> extrahiert alle führenden whitespaces
        getline(iss, dummy, '"'); //' lies aus iss solange in dummy bis du ein " findest '

        //erste Station der Linie
        if (!getline(iss, prevStation, '"')) continue; //versuche in prevStation bis zumnächsten " einzulesen, wenn das nicht klappt (e.g. du findest kein "), dann überspringe diesen Loop -> gehe zur nächsten Zeile im File

        //alle weiteren Stationen der Linie
        while (iss >> weight) //jetzt wird das Gewicht der Verbindung zwischen den beiden Stationen, eingelesen, da weight ein Int ist, wird nur so lange eingelesen wie Zahlen vorhanden sind. (also nur das Gewicht)
        {
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
printNeighborsStation(graph["Westbahnhof"]);


///////////////////////////////////////////////////
//              Finden einer Route               //
///////////////////////////////////////////////////


    return 0;
}


//FUNKTIONSDEFINITIONEN

string trim(const string& str)
{
    const char* whitespace = " \t\n\r"; //gibt an Welche Zeichen als "whitespace" gelten
    size_t start = str.find_first_not_of(whitespace); //sucht das Erste Zeichen das kein "Whitespace" ist
    if (start == string::npos) return ""; //string::npos = "bis zum Ende des Strings"
    size_t end = str.find_last_not_of(whitespace); // posititon des letzten Zeichens das kein "whitespace" ist.
    return str.substr(start, end - start + 1); //wir geben nur den String zwischen start und end aus. (aber start soll ja dabei sein, deshalb end-start+1
}

void printGraphByLine(const unordered_map<string, vector<EdgeInfo>>& graph)
{
    // Gruppierung nach Linien zum Ausgabezweck
    unordered_map<string, set<pair<string, string>>> linienGraph;

    for (const auto& entry : graph)
    {
        const string& from = entry.first;
        for (const EdgeInfo& edge : entry.second)
        {
            // alphabetisch sortieren, um Duplikate zu vermeiden
            string s1 = from, s2 = edge.to;
            if (s1 > s2) swap(s1, s2);
            linienGraph[edge.line].insert({s1, s2});
        }
    }

    // Ausgabe nach Linien gruppiert
    for (const auto& entry : linienGraph)
    {
        const string& linie = entry.first;
        cout << linie << ":\n";
        for (const auto& conn : entry.second)
        {
            const string& s1 = conn.first;
            const string& s2 = conn.second;
            int weight = -1;

            // Suche Gewicht in graph[s1]
            const auto& edges = graph.at(s1);
            for (const auto& edge : edges)
            {
                if (edge.to == s2 && edge.line == linie)
                {
                    weight = edge.weight;
                    break;
                }
            }

            cout << "  " << s1 << " <-" << weight << "-> " << s2 << endl;
        }
        cout << endl;
    }
}

void printNeighborsStation(const vector<EdgeInfo>& neighbors)
{
    if (neighbors.empty())
    {
        cout << "Keine Nachbarstationen gefunden.\n";
        return;
    }

    cout << "Nachbarstationen:\n";
    for (const auto& edge : neighbors)
    {
        cout << "  -> " << edge.to
             << " (Linie: " << edge.line
             << ", Gewicht: " << edge.weight << ")\n";
    }


}
