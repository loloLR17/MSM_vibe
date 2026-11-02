# Preuves Shield V1.1

Mission CAD exclusivement. Départ d0433fb5352d857344d892ed86099df6a09a8f25. V1 d’origine au commit1d2aa6683e972a3751f69229db4022283f55b3fb et bilan44fa2c68 préservés ; aucune archive V1 ni firmware/média/banc modifié.

Les contrôles finaux se trouvent dans `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/docs/checks/` : ERC/DRC complets avec parité, relecture PCB, netlist/CAM/perçages et contrôle mécanique nominal. Les commandes OpenSCAD sont dans `mechanical/REVIEW_ONLY/commands.json`, avec logs par pièce. Les bibliothèques manquantes de l’AppImage ont été résolues par extraction locale de paquets Debian ; aucun outil n’a touché une cible STM32.

Les modèles STL sont fermés et de volume positif, pas physiquement qualifiés. Les zones de doigts/câbles, hauteurs et surfaces d’appui sont des propositions. Le calcul du détrompage Click porte sur le corps v101, pas sur la révision réelle v102 ; le détrompage MKI reste bloqué faute de forme asymétrique relevée. Les centres LED/USER et ouvertures USB exactes C05 restent inconnus.

Une première interprétation de photo a envisagé à tort une inversion du sens SD. La feuille2 du dessin4UCON15882 a confirmé le sens-X déjà correct ; aucune rotation n’a été appliquée. La seule modification de position SD est(-3,5;0)mm. Les confirmations15,2mm d’entraxe MKI/GND bas droit sont préservées et non redemandées.

Exports texte normalisésLF pour Git après génération, sans modification des binaires. Les vérifications d’exports sont refaites sur les fichiers publiés. Les originaux, modèles SD constructeur et PDF complets restent dans reports local ; leurs hashes et provenances sont dans docs/sources-v1-1.json. Le gabarit/plan et les vues STL ont été inspectés visuellement ; pas de prétendue simulation complète d’un assemblage C05 réel.

La comparaison du projet a détecté une réinitialisation involontaire de seuils par le générateur schématique. Le projet V1 a été préservé, le générateur ne le réécrit plus, puis ERC/DRC/parité/exports ont été relancés avec min_clearance0,20/min_track_width0,25/min_via_diameter0,60. Les premiers résultats sous valeurs par défaut ne sont pas le gate final. L’avenant77d67460 a été intégré sans résolution de divergence (aucun commit local de mission créé à ce stade), dans un nouveau worktree isolé ; tous les travaux source sont conservés.
