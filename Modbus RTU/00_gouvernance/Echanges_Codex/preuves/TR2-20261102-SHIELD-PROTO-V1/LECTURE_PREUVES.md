# Preuves SHIELD-PROTO-V1

Les fichiers initiaux de ce dossier conservent les erreurs de développement CAO, **pas l’état final**. Le DRC initial repère39 anomalies, les premières tentatives de routage échouent sur géométrie/obstacles ; aucune de ces erreurs n’est masquée dans la conclusion. Un essai conservateur sans passage a aussi été arrêté ; les routes finales viennent du calcul géométrique et du DRC natif.

État final faisant foi : [contrôles finaux](../../../../04_Architecture/SHIELD_PROTO_V1/docs/checks/), en particulier ERC.json, DRC.json, commands.json, pcb-readback.json et export-verification.json. Voir le dossier CAO depuis la racine `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/`.

Inspection visuelle : schéma PDF lisible et image de la carte nue examinés. Aucune vue 3D complète Nucleo/modules, aucune mesure d’assemblage ni de tenue physique. Les erreurs de parité NC ont été corrigées en reprenant les117 noms d’îlots non connectés de la netlist ; aucun contrôle n’a été désactivé pour les faire disparaître. Les courtyards sont ceux des sockets, les enveloppes élevées F.Fab sont soumises au contrôle d’empilage documenté.

Aucun flash/debug/accès COM/SD/intervention matérielle pendant cette mission. Les recherches constructeur en timeout sont décrites dans le manifeste ; elles ne justifient aucune cote inventée. Le composant C2 candidat UVR1C100MDD a été retiré de la BOM après contrôle fabricant (EOL/pas2mm). Les références exactes des passifs et fixations restent à figer.

Deux tentatives optionnelles de relecture native du gros import Altium ont été arrêtées après blocage de chargement ; aucune cible matérielle concernée. La positionU1 a été relue dans le fichier KiCad importé par un parseur S-expression Linux. Cette inspection secondaire ne remplace pas le contrôle du module réel.

Pour Git, les sorties texte publiées ont été normalisées UTF-8/LF, sans espaces de fin de ligne ; les sorties originales et tentatives restent dans le répertoire reports local. Aucun binaire PDF/PNG modifié. Les contrôles de comparaison CAM/perçages sont exécutés sur les fichiers effectivement publiés après normalisation.
