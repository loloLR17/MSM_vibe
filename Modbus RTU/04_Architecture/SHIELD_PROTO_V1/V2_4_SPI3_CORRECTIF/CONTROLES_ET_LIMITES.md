# E-01 et M-01 — preuves du candidat V2.4

## E-01 électrique

La référence est reconstruite depuis les seuls originaux fabricants, **avant lecture de la variante** : netlistPCAD C05 et projetST, UM2861 pour multiplexages, fichiersEagle officiels Adafruit FRAM4718/SD4682 épinglés à leurs commits, schéma ST MKIGIBV2 et schéma Clickv102. La politique de bus MCU est explicite ; aucune ancienne matrice projet ne sert de source de brochage.

- [Référence primaire et sources](preuves/E01/reference-constructeurs.json) :8sources,180contacts incluant tous les NC voulus, tensions/directions/masses/straps. Un aliasNC signifie ici « non utilisé par le shield », pas forcément un pad physiquement vide dans le fabricant.
- [Orientations et empreintes](preuves/E01/orientations-et-empreintes.json) :106positions nativesST recoupées (écartmax≈1,024nm),180pads de bibliothèques/empreintes placées, deux dessousZio/J5 compris,18pads+4trous Adafruit, DIL24J8,16pads mécaniques Clickv101.
- [Symboles/types](preuves/E01/symboles-types-180.json) :180pins externes/embarquées/exportées, directions des signaux connectés, nomsCS/SCL/SDA/SDO/NC23/24. Le lecteur traite explicitement le formatXML KiCad10 `nom_numéro` et le suffixe `+no_connect` ; il ne supprime ni pin ni comparaison.
- [Matrice180](preuves/E01/MATRICE_E01_180_CONTACTS.csv) et [matrice17nets](preuves/E01/MATRICE_E01_17_NETS.csv), [auditcomplet](preuves/E01/audit-complet.json) :180PASS,59contacts reliés,121NC,17nets. Les deux couches CAM ont leurs180flashes/pin/nets/positions/diamètres comparés au natif et à la source.
- Cuivre réellement connecté : union géométrique des flashes, traits et régions remplies Gerber, pontée seulement par les pads métallisés et4vias fixes.138composantes comprenant121NC isolés et17nets ; **zéro court-circuit et zéro coupure**. AttributsX2 contrôlés après ce calcul, pas utilisés comme simple preuve de connexion.
- Excellon : tous les centres/diamètresPTH,NPTH et4vias comparés. Le lecteur Gerber refuse toute commande non supportée, au lieu de l'ignorer. Coordonnées1µm, cercles approximés par128segments ; pas une métrologie usine.
- [Diffavant/après](preuves/diff-V23-V24.json) : six contactsJ8 seuls changent de net ; les six écarts constructeur deV2.3 deviennent zéro. Tous les segments hors4netsSPI3 et les4vias sont inchangés. Les7CAM horscuivres sont égaux après retrait des seules dates/LF.

### Accès constructeur et limites réelles

Le schémaST officiel `https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf` a été consulté viaoutilWeb le18/11 : version1/feuille1/figure2, sourceST,4pages. Les deux téléchargements directs Linux puisWindows ont expiré ; **aucun hash du PDF officiel téléchargé n'est inventé**. La transcription visuelle du dessinST DB4012Rev3/p2/Fig2 est recoupée depuis son miroir conservé et haché, avec corroboration de la page officielle ; CS19/SCL20/SDA21/SDO22,23/24sansliaison,1VDD/2VDDIO/13GND. Les détails de méthode/URL/version/hash du miroir sont dans la référence, pas maquillés en téléchargement officiel.

PourClick, v102 est la source **électrique** ; la CAO constructeurv101 est uniquement la preuve de géométrie16contacts. Ni cette similitude ni l'essai001 ne prouvent la géométrie complète d'unClickv102 nouveau. Straps3V3/J2/J7/J8/GND2 et rails réels restent des gates futurs, sans nouveau geste matériel pendant cette mission.

## M-01 mécanique

[Comparateur natif](tools/verify_m01.py) indépendant du générateur et de `design.json`. Il compare l'expressionnative complète en n'autorisant que les nets des sixpadsJ8, les segments des quatre netsSPI3 sur les deuxcuivres et les polygones de remplissage de plans dérivés. Aucun autre champ natif n'est supprimé de la comparaison : contour, stackup, épaisseur,191géométries de pads/trous,4vias,19empreintes/positions/angles/faces, corps, textes, sérigraphieARNACO, paramètres et modèles sont conservés. Bibliothèques d'empreintes, tables et projet restent byteexact.

[Résultatnatif](preuves/M01-native.json) ; [8contrôlesnégatifs](preuves/M01-negative-controls.json) rejetant en mémoire décalage1µm, via, perçage, épaisseur, logo, J9, perte d'unsegmentRTU et ajout d'un fauxnetACC. Aucun PCB n'est modifié par ces tests.

[STEPV2.7](preuves/M01-STEP-V27.json) : importlectureseule336solides,191axes/rayons de trous du shield comparés au candidat à10nm de résolution numérique,épaisseur1,6/G12,25 et3enveloppes de sockets retrouvées. Aucun STL/STEP/QUANTITES/PDFV2.7 régénéré. Tous les232fichiers V2.3/V2.7 initiaux ont leur empreinte vérifiée. Les4vias sont comparés exactement dans le natif ; le STEP simplifié ne représente pas leurs petits trous.

[Options de stock](preuves/stock-options-M01.json) : le STEP contient déjà les corps1×10 intacts FRAM/SD et les enveloppesAVX/Panasonic. Les références prévues entrent dans ces enveloppes nominales sans modifier le PCB. Contact10 hors des9trous à neutraliser au futur montage ; aucune manipulation réalisée. CourtyardC1AVX historiquement trop étroit de0,1875mm par côté, mais corps prévu dans l'enveloppeV2.7 ; réserve conservée, pas d'empreinte ajustée en silence.

**Réserve J5 détectée** : le petit corps simplifié du STEP mesure7,68mm longitudinalement contre8,13mm au Samtec ; écart0,225mm par extrémité. Le vrai corps nominal complet et la boîte des deux1×3 décrits par l'utilisateur ont été comparés en mémoire au reste de l'assemblage gelé : aucune intersection positive. Aucun modèlehistorique n'est « corrigé » pour obtenir unPASS. BudgetSamtec nominal4,48mm dans plage3,68..6,35 ; tolérancesV2.0 inchangées, pas d'insertion/tenue physique revendiquée. L'ancienJ5 était une adaptation1×3, pas une preuve deSamtec reçu.

## Ce qui reste avant fabrication et utilisation

Revue indépendanteChatGPT des sources, du nouveau schéma, des180contacts, desCAM et deM-01 ; décisionutilisateur pour commande. Puis montage à froid des pièces réellement reçues (PLA/ABS, hauteurs/tolérances, contacts, accès et câbles), gatesélectriques avant alimentation, qualificationSPI3 et intégration progressive. **Ces opérations ne sont pas exécutées dans la missionpureCAO.**

Ni les V2.3 fabriquées ni le défautJ8 historique ne sont réparés par ce dossier. Aucune libération électronique globale, qualification industrielle, tenueEMC/vibrations/thermique ou santé des périphériques n'est déclarée.
