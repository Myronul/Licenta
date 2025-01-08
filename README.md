Codul sursa in curenta dezvoltare se afla in: Core/Inc si Core/Src (modularizat in bibliotecile de functii)

Universitatea “Politehnica” din Bucureşti
Facultatea de Electronică, Telecomunicaţii şi Tehnologia Informaţiei
Program de studiu RST	Anexa 1
 
TEMA PROIECTULUI DE DIPLOMĂ
a studentului MIRON N.F. Andrei-Auraș, 444D
 
1. Titlul temei: Biblioteca de functii in limbaj C pentru transformarea imaginilor si efecte audio cu aplicatii pe microcontrolere

2. Descrierea temei:
Se va realiza o bibliotecă de funcții în limbaj de programare C pentru transformarea imaginilor (rotație, scalare, translatare) și realizarea de efecte audio. Funcțiile implementate vor fi testate în timp real prin realizarea practică a unui sistem compus din două subsisteme fiecare cu microcontroler ARM propriu: un subsistem pentru procesare grafică și audio și un subsistem de comandă a transformărilor grafice și a tipurilor de efecte audio. Sistemul realizat și biblioteca de funcții asociată se pot utiliza în aplicații multimedia cu microcontroler. Se vor utiliza cunoștințe matematice (geometrie, operații cu matrice), de prelucrare de semnal (generarea unui semnal eșantionat, filtrarea semnalelor) și de programare în limbaj de nivel înalt. Realizarea programelor se va face utilizând programul MATLAB (pentru validarea algoritmilor) și medii de dezvoltare a programelor pentru microcontrolere (STM32Cube IDE, STM32 CubeMx ) pentru generarea codului.

3. Contribuția originală:
1. Scrierea următoarelor funcții în limbaj C:
- generarea unor imagini geometrice simple
- rotația unei imagini în jurul unei axe
- translatarea unei imagini
- scalarea unei imagini
- generarea unor semnale audio sintetizate
- generarea unor efecte audio (ecou, reverberație, vibrato)

2. Proiectarea următoarelor module software:
- modul pentru conectarea și comanda ecranului LCD
- planificatorul de procese pentru procesele de prelucrare grafică, prelucrare audio, sincronizare și comandă
- protocolul de comunicație între subsistemele de procesare și de comandă


3. Evaluarea performanțelor sistemului
- timp de calcul
- rezoluție grafică
- calitatea semnalelor audio generate


4. Resurse și bibliografie:
1. Richard Szeliski, Computer Vision: Algorithms and Applications 2nd Edition, Springer 2022
2. Using the ADSP-2100 Family Volume 1 (Rev. 1.0); Chapter 9; https://www.analog.com/en/lp/001/adsp-manuals.html
3. https://users.cs.cf.ac.uk/Dave.Marshall/CM0268/PDF/10_CM0268_Audio_FX.pdf
4. https://www.researchgate.net/publication/236629475_Implementing_Professional_Audio_Effects_with_DSPs
5. STM32 Microcontrollers, https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html
6. a-Si TFT LCD Single Chip Driver 320(RGB) x 480 Resolution, 16.7M-color With Internal GRAM Specification
7. Note de curs (Sorin Zoican): Arhitectura Microprocesoarelor 2. Microcontrolere
8. Note de curs (Sorin Zoican): Sisteme de operare
