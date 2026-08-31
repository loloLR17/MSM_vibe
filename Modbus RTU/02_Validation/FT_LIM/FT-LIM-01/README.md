# FT-LIM-01 — Domaines unitaires des valeurs fonctionnelles

## 1. Objet

FT-LIM-01 valide les domaines fonctionnels **unitaires et explicitement normatifs** des champs RW du Bloc 4.

Cette sous-famille ne valide pas le droit d'écriture lui-même : ce point est gelé dans FT-ACC. Elle vérifie la conséquence fonctionnelle de la valeur écrite.

## 2. Doctrine

Pour un registre RW valide du Bloc 4 :

- une valeur fonctionnellement invalide reste une écriture Modbus valide ;
- la valeur peut être présente dans la configuration préparée ;
- toute modification de la zone préparée repositionne la configuration en `BROUILLON` ;
- une configuration contenant une valeur hors domaine ne doit pas être appliquée ;
- l'image active doit rester inchangée en cas d'échec de validation/application.

Aucune exception Modbus n'est attendue du seul fait qu'une valeur métier est hors domaine.

## 3. Périmètre actif

FT-LIM-01 couvre les règles unitaires statiques explicites de `bloc4.md` :

- identifiants non nuls requis pour validation ;
- fréquence d'échantillonnage ;
- masque d'axes ;
- pleine échelle ;
- mode d'acquisition ;
- taille de fenêtre ;
- période d'indicateur ;
- durée de campagne ;
- mode de stockage ;
- valeur nulle de `storage_limit_mb`.

La compatibilité dynamique de `storage_limit_mb` avec la capacité réelle du firmware est identifiée mais n'est pas entièrement démontrable par une borne statique.

## 4. Hors périmètre

Sont explicitement hors de FT-LIM-01 :

- droits RW/RO/réservés : FT-ACC gelée ;
- structure, longueur, typage, ASCII fixe : FT-STR gelée ;
- CRC et mécanique complète validation/application : future FT-LIM-03 ;
- contraintes entre plusieurs champs : future FT-LIM-02 ;
- commandes Bloc 5, temps Bloc 2, sélection campagne Bloc 6 : futures sous-familles dédiées ;
- domaines non définis normativement en V1.

Les exemples des sections « Compléments métier » ne sont jamais utilisés comme domaines normatifs.

## 5. Artefacts actifs

- `source/FT-LIM-01_source.md` : exigences sources normalisées ;
- `detaille/FT-LIM-01_detaille.md` : cas génériques ;
- `detaille/FT-LIM-01_matrice_domaines.csv` : classification exhaustive des 26 champs RW du Bloc 4 ;
- `instancie/FT-LIM-01_instancie_index.csv` : 68 instanciations actives ;
- `instancie/FT-LIM-01_instancie_overview.md` : synthèse de couverture ;
- `instancie/FT-LIM-01_execution.md` : procédure d'exécution commune.

## 6. Archives

`archive_pre_renforcement/` contient l'ancien FT-LIM-01 à titre historique.

Ces fichiers sont **NON EXÉCUTABLES**, **NON NORMATIFS** et exclus de toute matrice de couverture active.

## 7. Statut

Reconstruction V1 en cours d'audit croisé. Aucun gel n'est implicite tant que l'audit de la sous-famille n'est pas validé.
