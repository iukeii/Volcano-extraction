📘 Ugnikalnio Evakuacija – README 

Ugnikalnio evakuacija – tai objektinio programavimo principus demonstruojantis C++ konsolinis žaidimas.
Miestelyje įvyksta ugnikalnio išsiveržimas, o žaidėjas turi padėti gyventojams pasiekti saugias zonas, kol juos pasiekia plintanti lava.

Žaidėjas gali ribotai paveikti žemėlapį statydamas barikadas ir taip nukreipdamas gyventojų judėjimą ar sulaikydamas lavą.

Tikslas – išgelbėti kuo daugiau piliečių.

🎮 Žaidimo mechanika
Žemėlapio elementai:
Simbolis	Reikšmė
.	Kelias
H	Namas
S	Saugioji zona
V	Ugnikalnis
~	Lava
#	Barikada
C	Pilietis
🧠 Žaidimo taisyklės

Piliečiai prasideda namuose (H).

Jie automatiškai juda link artimiausios saugios zonos (S).

Lava plinta nuo ugnikalnio centre (V).

Žaidėjas per ėjimą gali atlikti iki 5 barikadų veiksmų:

pastatyti barikadą

nuimti barikadą

Barikados trukdo tiek lavos plitimui, tiek piliečių judėjimui.

Jeigu lava pasiekia pilietį – jis žūsta.

Jeigu pilietis pasiekia saugią zoną – jis išgelbėtas.

⚙️ Valdymas

Konsolėje prie kiekvieno ėjimo galima įvesti:

➤ Statyti / nuimti barikadą
B x y


Pvz.:

B 3 4

➤ Baigti ėjimą ir leisti pasauliui pajudėti

Tiesiog paspausti Enter ant tuščios eilutės.

➤ Išeiti iš žaidimo
Q

📊 Dėmenys ekrane

Žaidimas nuolat rodo:

Bendrą piliečių skaičių

Išgelbėtų piliečių skaičių

Gyvų ir dar nepasiekusių saugios zonos skaičių

Žemėlapio būseną

🧩 Sunkumo lygiai

Prie žaidimo pradžios galima pasirinkti:

Lengvas – lava plinta lėtai

Normalus – standartinis režimas

Sunkus – lava plinta greitai

Sunkumo lygis lemia, kiek kartų lava išplinta per vieną pasaulio ėjimą.

🛠️ Paleidimas
Kompiliavimas (Linux / macOS):
g++ -std=c++17 main.cpp -o evakuacija
./evakuacija

Kompiliavimas (Windows su MinGW):
g++ -std=c++17 main.cpp -o evakuacija.exe
evakuacija.exe

📂 Projekto struktūra (jei pateikiama darbui)
/
├── README.md
├── main.cpp
└── (papildomi failai, jei suskaidyta į klases)
