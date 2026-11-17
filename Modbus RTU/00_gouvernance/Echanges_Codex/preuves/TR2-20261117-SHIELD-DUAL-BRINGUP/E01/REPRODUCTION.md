# Reproduire les contrôles documentaires E-01

Ces outils ne communiquent avec aucune cible matérielle. Les originaux fabricants
restent dans `/home/lolo/dev/msm/reports/`, identifiés par les sources et hashes.
Les scripts écrivent seulement leurs sorties dans ce dossier E01. Les CAM, PCB,
schémas, médias et firmwares historiques ne sont jamais enregistrés par ces outils.

Depuis le worktree de cette mission :

1. `build_reference.py` avec Python Linux et sexpdata : construit la référence
   primaire à partir des seuls originaux fabricant et d’une politique fonctionnelle
   explicitée. Il ne lit aucun fichier KiCad ou matrice historique.
2. `read_native.py <PCB préservé> <sortie JSON>` avec le Python KiCad Windows :
   lecture native sansSave. L’export de netlist utilise `kicad-cli sch export netlist
   --format kicadxml -o <sortie E01> <schéma préservé>`. Les arguments et résultats
   réellement exécutés sont dans `native-commands.json`.
3. `audit_e01.py` avec Python Linux et Shapely : comparaison exhaustive et lecture
   géométrique des CAM/Excellon/ZIP existants. Il refuse les instructions Gerber
   non prises en charge ; il ne régénère aucun CAM pour le faire correspondre.
4. `audit_orientations.py` avec olefile et sexpdata : ST natif, géométrie de toutes
   les empreintes placées et sources Adafruit/Click, sans nouvelle qualification
   de la mécanique physique.

Les modules Linux déjà présents sont dans `/home/lolo/dev/msm/cache/shield-pylibs` ;
passer ce chemin par `PYTHONPATH` à chaque commande. Versions enregistrées dans
`sources-complementaires.json`. Outils Windows : KiCad10.0.6 sous
`C:\Program Files\KiCad\10.0\bin`. Les chemins WSL sont convertis par `wslpath -w`.
Aucun changement de politique Windows, installation système ou nouvelle permission
matérielle n’est nécessaire à ces contrôles.

Les anciens ERC/DRC et essais matériels ne sont pas relancés par cette procédure.
Leur portée reste distincte des nouveaux contrôles constructeur. Le champ `library`
de la lecture SWIG contient une représentation d’objet, pas un identifiant utilisé
comme vérité ; les noms et positions des bibliothèques sont comparés depuis leurs
expressions natives par `audit_orientations.py`.

La matrice chiffre les concordances documentaires, jamais une validation réelle
des180contacts soudés. Les remarques physiques conservées sont les mesures
utilisateur antérieures, avec leur portée limitée. Une contre-lecture indépendante
doit consulter les sources constructeurs, pas seulement réexécuter ces assertions.

`RAPPORT_AVANT_AVENANT_E01.md` est une copie byte exact : ses liens relatifs ont
pour contexte le chemin d’origine `Echanges_Codex/DERNIER_RAPPORT.md`, pas le nouveau
dossier d’archivage. Les résoudre depuis ce contexte ou consulter le
[rapport au commit7c70e7ea](https://github.com/loloLR17/MSM_vibe/blob/7c70e7ea4edfe5194198058ab03660073129f285/Modbus%20RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md).
Le premier contrôle automatique de liens a signalé ce changement de contexte ;
le validateur a ensuite utilisé le chemin original, sans modifier l’archive ni
supprimer son contrôle.
