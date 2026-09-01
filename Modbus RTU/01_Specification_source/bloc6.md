# TR2 — Bloc 6 : Inventaire campagnes



## Mapping Modbus RTU — V1



---



## 1. Objet du bloc



Le Bloc 6 permet à la centrale de :



* lister les campagnes enregistrées dans le capteur ;

* accéder aux métadonnées de chaque campagne ;

* suivre l’état de stockage ;

* préparer les opérations de récupération des données.



Ce bloc est majoritairement en lecture seule.

La sélection de la campagne courante via `selected_campaign_index` constitue la seule écriture autorisée, afin de permettre la navigation dans l’inventaire.



---



## 2. Base d’adressage



* **Bloc 6 base = registre 6000**



---



## 3. Accès



| Zone                  | Accès |

| --------------------- | ----- |

| Métadonnées globales  | RO    |

| Sélection campagne    | RW    |

| Entrée sélectionnée   | RO    |

| Informations stockage | RO    |

---



## 4. Conventions générales



* Types autorisés : `uint16`, `uint32`, `enum16`, `bitfield16`, `ASCII fixe`

* `uint32` = **MSW puis LSW**

* Lecture cohérente requise pour :



&nbsp; * timestamps

&nbsp; * tailles

&nbsp; * identifiants

* ASCII fixe :



&nbsp; * padding `0x00`

* Indexation contrôlée par registre de sélection

* Registres réservés lus à `0`



---



## 5. Philosophie du bloc



Le Bloc 6 ne contient **qu’une seule entrée visible à la fois**.



👉 La centrale sélectionne une campagne via un index :



* `selected_campaign_index`



👉 Le capteur expose ensuite les données correspondantes.



➡️ Cela permet :



* un bloc compact ;

* un accès déterministe ;

* une implémentation firmware simple.



---



## 6. États de campagne



### 6.1 `campaign_state`



| Valeur | Signification           |

| -----: | ----------------------- |

|      0 | vide                    |

|      1 | en préparation          |

|      2 | en cours                |

|      3 | terminée                |

|      4 | erreur                  |

|      5 | partiellement corrompue |

| 6..65535 | réservés              |



---



## 7. Mapping complet



### 7.1 Métadonnées globales



| Offset | Adresse absolue | Champ                       | Type      | Accès | Description               |

| -----: | --------------: | --------------------------- | --------- | ----- | ------------------------- |

|      0 |            6000 | inventory_structure_version | uint16    | RO    | Version structure Bloc 6  |

|      1 |            6001 | total_campaign_count        | uint16    | RO    | Nombre total de campagnes |

|      2 |            6002 | valid_campaign_count        | uint16    | RO    | Nombre campagnes valides  |

|      3 |            6003 | selected_campaign_index     | uint16    | RW    | Index sélectionné         |

|      4 |            6004 | selected_campaign_valid     | uint16    | RO    | 0 = invalide / 1 = valide |

|    5-6 |       6005-6006 | storage_used_mb             | uint32    | RO    | Espace utilisé            |

|    7-8 |       6007-6008 | storage_free_mb             | uint32    | RO    | Espace libre              |

|      9 |            6009 | storage_health_status       | uint16    | RO    | État stockage             |

|  10-11 |       6010-6011 | reserved_6A                 | uint16[2] | RO    | Réservé                   |



---



### 7.2 Entrée campagne sélectionnée



| Offset |   Adresse | Champ                 | Type       | Accès | Description          |

| -----: | --------: | --------------------- | ---------- | ----- | -------------------- |

|  12-13 | 6012-6013 | campaign_id           | uint32     | RO    | Identifiant campagne |

|  14-15 | 6014-6015 | mission_id            | uint32     | RO    | Identifiant mission  |

|  16-17 | 6016-6017 | start_timestamp       | uint32     | RO    | Début campagne       |

|  18-19 | 6018-6019 | end_timestamp         | uint32     | RO    | Fin campagne         |

|     20 |      6020 | campaign_state        | enum16     | RO    | État campagne        |

|  21-22 | 6021-6022 | duration_s            | uint32     | RO    | Durée                |

|  23-24 | 6023-6024 | data_size_mb          | uint32     | RO    | Taille données       |

|  25-40 | 6025-6040 | campaign_label        | ASCII fixe | RO    | Label campagne       |

|  41-56 | 6041-6056 | mission_label         | ASCII fixe | RO    | Label mission        |

|     57 |      6057 | data_integrity_status | uint16     | RO    | Intégrité données    |

|  58-63 | 6058-6063 | reserved_6B           | uint16[6]  | RO    | Réservé              |



---



## 8. Détails des champs critiques



### 8.1 `selected_campaign_index`



* index logique dans la liste interne

* plage :



&nbsp; * `0` → première campagne

&nbsp; * `N-1` → dernière campagne

* hors plage → `selected_campaign_valid = 0`

* une valeur de `selected_campaign_index` hors de la plage `0 .. N-1` est une valeur sémantiquement invalide, mais reste une écriture Modbus valide sur le registre `6003` ;
* dans ce cas :
  * l’écriture est acceptée ;
  * `selected_campaign_valid = 0` ;
  * aucune exception Modbus ne doit être générée du seul fait que l’index est hors plage ;
  * les métadonnées de campagne sélectionnée ne doivent pas être interprétées comme valides tant que `selected_campaign_valid = 0`.



---



### 8.2 `selected_campaign_valid`



| Valeur | Signification   |

| -----: | --------------- |

|      0 | index invalide  |

|      1 | campagne valide |



---



### 8.3 `campaign_id`



* ne doit jamais être `0` pour une campagne valide ;
* deux campagnes distinctes présentes dans l’inventaire d’un même TR2 doivent avoir des valeurs de `campaign_id` distinctes ;
* aucune unicité inter-capteurs n’est imposée par la V1 ;
* côté centrale, l’identification globale d’une campagne repose sur le couple `(device_id, campaign_id)`.



---



### 8.4 `start_timestamp` / `end_timestamp`



* base temporelle = Bloc 2

* `end_timestamp = 0` si campagne en cours



---



### 8.5 `duration_s`



* pour une campagne terminée dont `start_timestamp` et `end_timestamp` appartiennent à une base de temps continue sans resynchronisation affectant l’intervalle : `duration_s = end_timestamp - start_timestamp` ;
* pour une campagne en cours (`end_timestamp = 0`), la V1 ne définit pas de relation arithmétique obligatoire entre `duration_s` et les timestamps ;
* pour une campagne traversant une discontinuité de la base de temps due à une resynchronisation, la V1 ne définit pas de relation arithmétique obligatoire entre `duration_s` et les timestamps ;
* dans ces deux derniers cas, la centrale ne doit pas déduire une durée par simple soustraction des timestamps.



---



### 8.6 `data_size_mb`



* taille logique des données

* utilisée pour :



&nbsp; * supervision stockage

&nbsp; * estimation transfert



---



### 8.7 `data_integrity_status`



| Valeur | Signification |

| -----: | ------------- |

|      0 | inconnue      |

|      1 | OK            |

|      2 | corrompue     |

|      3 | partielle     |



---



### 8.8 `storage_health_status`



| Valeur | Signification |

| -----: | ------------- |

|      0 | OK            |

|      1 | warning       |

|      2 | dégradé       |

|      3 | critique      |



---



## 9. Règles d’implémentation



* seule l’écriture de `selected_campaign_index` est autorisée dans le Bloc 6 ;
* toute tentative d’écriture sur un autre registre du Bloc 6 doit être rejetée par une exception Modbus explicite ;
* aucune écriture silencieusement ignorée ou partiellement exécutée n’est autorisée ;
* la sélection de la campagne exposée est pilotée par la valeur écrite dans `selected_campaign_index`.

* cohérence exigée :



&nbsp; * timestamps

&nbsp; * tailles

&nbsp; * état

* lecture répétable (pas de glitch entre registres)

* index stable pendant une lecture



### Cohérence de sélection pendant la lecture



Le firmware doit garantir que la campagne sélectionnée reste figée pendant une réponse Modbus multi-registres couvrant le Bloc 6.



En particulier :

- les champs de l’entrée sélectionnée doivent provenir d’une seule et même campagne ;

- un changement de `selected_campaign_index` ne doit pas produire un mélange de deux campagnes dans une même lecture ;

- si nécessaire, le firmware doit utiliser un snapshot interne des métadonnées de campagne sélectionnée.



---



## 10. Justification



Ce design permet :



* navigation simple depuis la centrale ;

* faible complexité firmware ;

* robustesse Modbus ;

* évolutivité vers transfert automatisé futur.



---



## 11. Réserves / extensions futures



* pagination multi-entrées ;

* indexation par timestamp ;

* ajout CRC campagne ;

* ajout chemin fichier / identifiant physique ;

* ajout statut de transfert.



Les zones réservées permettent ces extensions.





---

# Compléments métier (ajoutés)

> **Statut des compléments métier**
>
> Les sections « Compléments métier » sont informatives et non normatives.
> En cas de divergence, les définitions, types, codes, domaines et règles
> figurant dans les sections normatives du présent document font foi.
> Les « valeurs recommandées » mentionnées dans les compléments ne constituent
> pas des codes protocole tant qu’elles ne sont pas intégrées explicitement
> au mapping normatif.

## Nombre de campagnes disponibles

Utilisation métier
1. connaître immédiatement le volume de données stockées
2. détecter une absence de données ou un problème d’enregistrement

Exemple
0 campagne → capteur non utilisé ou défaut acquisition.

## Index campagne courante / sélectionnée

Utilisation métier
1. naviguer dans les campagnes
2. sélectionner une campagne pour lecture ou extraction

Contrainte
L’index doit être stable pendant la lecture des métadonnées associées.

## Identifiant unique de campagne

Utilisation métier
1. garantir l’unicité d’une campagne
2. permettre un suivi multi-capteurs
3. éviter toute confusion lors de l’exploitation

Exemple
Deux capteurs différents peuvent participer à une même mission mais avec des campagnes distinctes.

Contrainte
L’identifiant doit être unique au minimum à l’échelle du capteur.

## Timestamp début campagne

Utilisation métier
1. positionner la campagne dans le temps
2. corréler avec navigation ou événements

Exemple
Début campagne = départ du port, mise en charge machine, etc.

## Timestamp fin campagne

Utilisation métier
1. définir la durée de la campagne
2. détecter une campagne interrompue

Exemple
Une campagne sans timestamp de fin peut indiquer un arrêt brutal.

## Durée campagne

Utilisation métier
1. vérifier la cohérence avec la mission
2. comparer différentes campagnes

Exemple
Une durée anormalement courte peut indiquer un problème d’acquisition.

## Taille des données

Utilisation métier
1. estimer le volume de données à extraire
2. détecter une anomalie (taille incohérente)

Exemple
Une taille très faible peut indiquer une campagne vide ou invalide.

## Nombre d’échantillons / enregistrements

Utilisation métier
1. vérifier la cohérence avec la fréquence d’échantillonnage
2. détecter des pertes de données

Exemple
Nombre incohérent → problème acquisition ou stockage.

## Statut de la campagne

Utilisation métier
1. savoir si la campagne est exploitable
2. détecter une anomalie ou une interruption

Valeurs recommandées
1. en cours
2. terminée proprement
3. interrompue
4. invalide

Exemple
Une campagne interrompue doit être analysée avec prudence.

## CRC / intégrité campagne

Utilisation métier
1. vérifier que les données n’ont pas été corrompues
2. sécuriser l’exploitation

Exemple
Une CRC invalide doit empêcher une analyse automatique.

Contrainte
Le calcul doit être documenté et reproductible.

## Copie du contexte de campagne (Bloc 4)

Utilisation métier
1. figer le contexte au moment de l’acquisition
2. garantir la traçabilité complète

Contenu typique
1. mission_label
2. operating_mode
3. navigation_zone

Exemple
Deux campagnes avec la même configuration technique mais des contextes différents doivent rester distinguables.

Contrainte
Ces informations doivent être immuables une fois la campagne terminée.

## Copie de la configuration utilisée

Utilisation métier
1. savoir exactement comment les données ont été acquises
2. permettre une analyse reproductible

Exemple
Fréquence, axes, mode acquisition doivent être conservés.

Contrainte
Ne pas dépendre de la configuration actuelle du capteur.

## Indicateurs synthétiques campagne

Utilisation métier
1. obtenir une vue rapide sans charger les données brutes
2. trier les campagnes à analyser en priorité

Exemples
1. RMS moyen
2. RMS max
3. nombre d’événements

Exemple
Permet d’identifier rapidement une campagne anormale.

## Présence d’événements

Utilisation métier
1. détecter une activité anormale
2. prioriser l’analyse

Valeurs recommandées
0 = aucun
1 = présent

Exemple
Campagne avec événements → analyse prioritaire.

## Compteur de fichiers / segments

Utilisation métier
1. gérer les campagnes fragmentées
2. vérifier la complétude des données

Exemple
Une campagne longue peut être découpée en plusieurs fichiers.

## Validité globale campagne

Utilisation métier
1. filtrer les campagnes exploitables
2. éviter des analyses sur données corrompues

Valeurs recommandées
0 = invalide
1 = valide

Exemple
Campagne invalide → ignorée en traitement automatique.

## Traçabilité et reproductibilité

Une campagne doit être entièrement interprétable sans dépendre :
1. de l’état actuel du capteur
2. de la configuration active
3. de la centrale

Conséquence
Toutes les informations nécessaires à l’analyse doivent être embarquées dans les métadonnées de la campagne.

Objectif
Garantir une analyse fiable même plusieurs mois après acquisition.

## Position dans le système

Le Bloc 6 constitue la porte d’entrée vers les données stockées.

Utilisation
1. navigation dans les campagnes
2. sélection pour extraction (carte SD ou futur bus)
3. tri et priorisation

Limitation
Le Bloc 6 ne contient pas les données brutes, uniquement les métadonnées.
