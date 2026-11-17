# E-01 — revue électrique complète du shield V2.3

Mission `TR2-20261117-SHIELD-DUAL-BRINGUP`, révision2, reprise à `6b208d29`.
**Décision technique : NO-GO alimentation/libération.** L’audit documentaire est
réalisé ; le défaut SPI3 reste présent dans les cartes et les fichiers historiques.
La contre-lecture indépendante de ChatGPT et les contrôles matériels restent requis.

## Méthode et couverture

La [référence constructeur](reference-constructeurs.json) est produite par
`build_reference.py` **sans lire de fichier KiCad, design.json ou matrice interne**.
Les choix de fonctions MCU proviennent des drivers TR2 existants ; ils sont une
politique de raccordement, pas une source de numérotation des breakouts.

Sources séparées : ST MB1549 C05 netlist et PcbDoc natifs ; fichiers Eagle Adafruit
figés ; dessin ST du MKIGIBV2 ; schéma électrique Click v102. URL, version et SHA-256
sont conservés. Le téléchargement direct du PDF ST a encore expiré : le document
ST DB4012 Rev3 effectivement rendu/lu provient du miroir identifié Xon. Ce PDF n’est
pas présenté comme un téléchargement officiel. Le schéma officiel ST reste le lien
de contre-lecture. Les sources complètes ne sont pas redistribuées dans ce dossier.

| Source primaire | Artefact effectivement lu | Usage / limite |
|---|---|---|
| [ST MB1549 manufacturing](https://www.st.com/resource/en/board_manufacturing_specification/mb1549-manufacturing.zip) | MB1549-C05_PCAD.NET, SHA `1dc0c13ea1bb8a5990475d35ed4161c44713d45369ca23369533671b835951d3` |100 contacts Zio et6 positions CN12 ; noms de nets et ponts identifiés |
| [ST MB1549 BDP](https://www.st.com/resource/en/board_manufacturing_specification/mb1549-bdp.zip) | PcbDoc SHA `acb801385195543d1471c6bf34cd278865449e0fc15c5546173782455c1749bf` ; projet C/05 | lecture native des coordonnées, variante U575ZIQ ; pas de modèle générique substitué |
| [Adafruit4718](https://github.com/adafruit/Adafruit-FRAM-Breakout-PCB/tree/f7d4afa22ddd562f52c557e2e8f003611515b4f4) | Eagle BRD et SCH officiels figés |9 contacts, VIN, sortie LDO, buffers et rappels embarqués |
| [Adafruit4682](https://github.com/adafruit/Adafruit-MicroSD-SPI-or-SDIO-card-breakout-PCB/tree/959792478854acec21fb862650427ffaa32b07f1) | Eagle BRD et SCH officiels figés |9 contacts, support SD, rappels, domaine3V3 |
| [ST MKI208V1K](https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf) | dessin ST DB4012 Rev3 p2 Fig2 via miroir, SHA `182b5cd300e0639bf6764c5e409aa0e88574e0aed7b1abff90f371eaaa85fc90` |24 contacts ; circuit MKIGIBV2, J1 et nappe vers MKI208V1 ; résistances33Ω côté capteur |
| [MikroE Click v102](https://download.mikroe.com/documents/add-on-boards/click/rs485_lsolator_2_click/rs485-isolator-2-click-schematic-v102.pdf) | PDF SHA `204f5c4ba323457f7e08c0c44c0f3a025cad8d06ca73ffc189fd46c73d8b716c`, retéléchargé identique |16 contacts, ADM2867E et straps ; v101 mécanique séparé |

La matrice [180 contacts CSV](MATRICE_E01_180_CONTACTS.csv) fournit pour **chaque**
pad source/fonction/direction/rail, net attendu, pin du symbole, net du schéma,
PCB et deux cuivres CAM, conséquence et limite physique. Les 180 contacts sont :
J1=20, J2=16, J3=30, J4=34, J5=6, J6=9, J7=9, J8=24, J9=16,
R1..R4=8, C1/C2=4, TP1..TP4=4. Les11 perçages mécaniques non numérotés sont
contrôlés séparément ; ils ne sont pas des contacts électriques oubliés.

**174 PASS documentaires,6 FAIL ; aucune broche omise.** PASS signifie uniquement
concordance du raccordement numérique avec la référence indiquée, sans qualification
électrique sous tension ou métrologique des deux assemblages. Les17 nets fonctionnels
sont également récapitulés [individuellement](MATRICE_E01_17_NETS.csv) ;121 pads restent
des îlots NC volontaires dans le PCB actuel. Un NC projet peut être une entrée ou
un rail réel du module délibérément non raccordé : ces deux notions sont distinguées.

## Six écarts bloquants

| Contact J8 / MKIGIBV2 | Fonction constructeur | Attendu | Schéma / PCB / CAM actuels | Verdict |
|---|---|---|---|---|
|19| CS |ACC_CS| NC |FAIL : sélection capteur absente |
|20| SCL |ACC_SCK| NC |FAIL : horloge absente |
|21| SDA |ACC_MOSI| ACC_CS |FAIL : mauvaise source |
|22| SDO |ACC_MISO| ACC_SCK |FAIL : horloge sur sortie capteur |
|23| NC |NC| ACC_MOSI |FAIL : sortie MCU sans destination utile |
|24| NC |NC| ACC_MISO |FAIL : entrée MCU sans source utile |

Sur001, observations utilisateur hors tension : J2-6→TP3=0,3Ω ; TP3→SDO=0,3Ω
et TP3→SCL≈600kΩ. La liaison erronée de l’horloge est donc aussi établie sur le
matériel001. Les autres résistances élevées restent des observations de chemins
incorrects, pas une preuve de quatre mauvaises soudures. Même conception pour002,
sans prétendre avoir mesuré ce segment sur002. Une contention sortie MCU/SDO est
possible si SDO est actif ; aucun dommage au capteur n’est démontré.

Les libellés/types de pins du symbole contribuaient aussi à masquer l’écart :
19/20 portent des marqueursNC alors qu’ils sont requis ;22 est typé entrée dans
le projet alors que le constructeur y placeSDO, une sortie ;23/24 portent les
types des signaux imaginés au lieu desNC réels. Ce sont des conséquences du même
référentiel fautif, pas des défauts corrigés par le succès de l’ERC.

## Autres interfaces et risques contrôlés

| Chemin / point | Constat documentaire | Limite matérielle / décision |
|---|---|---|
| Nucleo CN7..10 et CN12 |106 positions recoupées au PcbDoc ST ; erreur maximale1,024nm après même transformation rigide des repères |contacts, soudures et engagement réels non tous mesurés |
| J5 sous shield |local1..6 = CN12-65..70 ; seule local5/69 PG4 surRTU_DE ; autresNC ; pas de miroir de rangée |présence et continuité versClick encore INCONNUES ; absence interditTX |
| LPUART |PG7→CN10-14 viaSB25, PG8←CN10-16 viaSB27 ; SB31/SB33 sont exclusions de la variante U575ZIQ du projet ST |état réel des straps non relu ; pas d’hypothèse de configuration imposée aux autres variantes |
| GPIOG et niveaux |UM2861 indique une alimentation d’E/S distincte pourPG2..15 ; PG4/7/8 doivent être compatibles avec le domaine logique3V3 duClick |VDDIO2 réel non mesuré ; une tension correcte àTP1 ne remplace pas ce contrôle de domaine |
| FRAM J6 |JP1-9 VIN reçoit3V3 ;7GND ;8 sortie AP2112 non réinjectée ;3CS,4MOSI,5MISO,6SCK ;1HOLD/2WP laissés au rappel embarqué |référence physique exacte, tension après LDO et marges non mesurées ; FAIL001 à1MHz conservé, PASS250kHz borné |
| FRAM protections |Eagle SCH : réseau R1 10k tire CS/WP/HOLD versVIN ; U3 74HC4050 et AP2112 ; connexions vérifiées jusqu’aux mémoires via nets3V3 |peuplement réel / santé des composants non affirmés depuis le fichier fabricant |
| SD J7 |1=3V3,2GND,3CLK,4DO,5DI,6CS ;7DAT1/8DAT2/9détection non utilisés enSPI ; packs47k embarqués |carte32Go uniquement ; pas de5V/tolérance5V implicite ; PASS court001 ne qualifie pas002 |
| Click J9-3 |CS mikroBUS porteDE, pasPWM ; chemin PG4→J5→J9-3 cohérent numériquement |ancien libellé «PWM/DE» erroné, mais routage vers3 correct ; J5 physique à établir |
| Click UART |J9-13 mène àTXD11 (entrée ADM), donc reçoitTX MCU ; J9-14 vient deRXD8 (sortie ADM), donc mène àRX MCU |labels TX/RX du module et du MCU ne sont pas interchangeables ; pas de testModbusdéduit |
| Click /RE |position2/RST viaJ3 vers/RE ; pontJ2 relieDE et/RE, déclaré par utilisateur |J3 n’est pas un strap à la masse ; ne pas demander son retrait sans raison ; état actuel à confirmer |
| Click inversion |15INT/INVD et16PWM/INVR non pilotés ; R15/R16 6,8k tirent bas |ponts réels et éventuels dommages non vérifiés |
| Click VCCSEL |J1 du schéma, nomméJP1 dans la table produit : sélectionner3V3 ; J9-10/5V NC ; VIO au3V3 |position physique actuelle INCONNUE ; 3V3 mesuré au breakout ne démontre pas ce strap |
| Click half-duplex |J7 relieA/Y ; J8 relieB/Z, côté isolé |pontJ2 DE-/RE ne remplace pas ces deux ponts ; leur état et la polarité convertisseur restent inconnus |
| Click terminaison |R4=330Ω viaJ5 ; R5//R6=165Ω viaJ9 ; deux straps≈110Ω ; ni strap=circuit ouvert |aucune terminaison120Ω supposée ; choix selon bus réel, sans modification maintenant |
| Click bias |J4 sélectionne chemin versVCC2, J6 versGND2, R7/R14=120k et R8/R13=330 selon schéma |pas de polarisation externe supposée, pas de garantie de marge au repos sans bus réel |
| Isolation |GND2, A/B/Y/Z, VISOIN/VISOOUT restent surClick ; aucun netGND2 du shield ; tous GND J9-8/9 sontGND1 |isolement galvanique réel, défaut de soudure, câbles et boîtier non qualifiés ; aucune liaisonGND2↔GND à ajouter |
| R1/R2/R3 |10k CS au3V3 ; états inactifs souhaités au boot ; R1 tire aujourd’hui le mauvais contact21 du capteur |rappel du vraiCS19 absent : correction requise avant activitéSPI3 |
| R4 |10k DE àGND ; drive prévu inactif pendant init |ne prouve pas le niveau réel du Click siJ5 absent ou strapincorrect |
| C1/C2 / rails |100n et10µF entre3V3/GND ; C2 pad1 positif ; pas de net5V/VIN/GND2 distribué |valeurs et polarité des pièces réelles, bilan courant/ripple/marge3V3 et démarrages restent à mesurer |
| TP1..4 |3V3, GND, SCK, SD_CS cohérents avec schéma ; TP3 versSDO confirme le défaut final |ne pas confondre fonction du net et signal effectivement raccordé au breakout |

Ces vérifications ne mettent pas en évidence une deuxième erreur d’affectation
électrique dans les fichiers contrôlés. Elles **ne déclarent pas les autres modules
physiquement conformes** : configurations, continuités et alimentation réelle ne sont
pas couvertes par la parité numérique.

## Orientations, bibliothèque et CAM

`orientations-et-empreintes.json` documente : lecture binaire ST native ;180 pads
des bibliothèques placées recoupés avec les pads du PCB, retournement B.Cu inclus ;
18 contacts Adafruit et4 trous de fixation depuis leurs Eagle, avec transformation
top-view cohérente ; DIL24 MKIGIBV2 de1..12 à gauche et24..13 à droite ;16 pads
du **PcbDoc Click v101** alignés à la rotation retenue. Les formats x/y et y-haut/y-bas
sont explicités ; leur changement de signe n’est pas un miroir physique du module.
Le natif Click v102 n’est pas disponible : l’identité exacte de sa géométrie avecv101
reste INCONNUE, même si le schéma v102 et l’interface standard ont été recoupés.

Le PCB historique SHA `b79f2d6b1a0b44f2269087dcbdc6ba7c4ec9f5f41c704e4ebf77089f2d1729ab`
n’a pas changé. Les deux cuivres sont reconstruits depuis **tous** les flashes,
traits et régions, sans se limiter aux attributs X2.138 groupes connectés après
liaison des PTH et4vias :17 nets et121 îlotsNC. Aucun court-circuit ou ouvert
supplémentaire vsnetsPCB. Les cercle128segments et coordonnées1µm limitent ce calcul ;
il ne mesure pas la gravure ni une isolation certifiée. Tous perçages comparés aux
Excellon ; aucun ERC/DRC déjà acquis rejoué pour ce contrôle nouveau.

Le ZIP9membres conservé a SHA `e91d5ce68ee4339b44ada627fd8fc5dc097b4e9b2a128b15611dd996a6e44d17`.
La revue ChatGPT V2.3 avait autorisé le téléversement de cette archive, la préparation
du devis et la soumission à l’utilisateur d’une commande de5prototypes ; son hash
ZIP était alors annoncé par manifeste, non recalculé indépendamment parChatGPT ;
aucun reçu de commande/upload attestant que ce hash fut effectivement téléversé
n’est disponible. Le rapprochement **fichier approuvé→fichier conservé** est établi,
**fichier conservé→lot fabriqué réel** reste inconnu. La mesure001 apporte une
corroboration physique du défaut, pas l’identité complète du lot.

## Gates encore ouverts

1. Contre-lecture indépendante ChatGPT des sources, de cette matrice et de la RCA.
2. Décision sur reprise/interposeur/refabrication après [étude de réemploi](REEMPLOI_ET_OPTIONS.md).
3. Aucun nouvel essai alimenté avant correction revue et contrôles hors tension.
4. RepriseGND002 à confirmer avec continuités/absencecourt-circuit ; elle ne corrige pasSPI3.
5. Révisions et peuplement réels des breakouts, variante1x10 J6/J7 et neutralisation
   du contact supplémentaire, J5/Click/straps, marges de rail et contacts restent
   à établir ; pas de demande de manipulation pendant cet audit.
6. WHO unique puis configuration/XYZ, RS485 et reprise sélective des validations
   affectées restent futurs et conditionnels. Ne pas rejouer les acquis inchangés.

**Aucune correction CAO/CAM, dépense ou intervention matérielle exécutée par cet audit.**
