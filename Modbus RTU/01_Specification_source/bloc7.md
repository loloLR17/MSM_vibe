# TR2 — Bloc 7 : Diagnostic



## Mapping Modbus RTU — V1



---



## 1. Objet du bloc



Le Bloc 7 expose les informations de **diagnostic interne du capteur**.



Il permet à la centrale de :



* surveiller l’état de santé du capteur ;

* détecter les défauts critiques et non critiques ;

* analyser les causes d’anomalies ;

* suivre les résultats d’autotest ;

* accéder à des indicateurs techniques internes.



Ce bloc est **strictement en lecture seule**.



---



## 2. Base d’adressage



* **Bloc 7 base = registre 7000**



---



## 3. Accès



| Zone               | Accès |

| ------------------ | ----- |

| Diagnostic courant | RO    |

| Détails défauts    | RO    |

| Résultats autotest | RO    |

| Compteurs internes | RO    |



---



## 4. Conventions générales



* Types autorisés : `uint16`, `int16`, `uint32`, `bitfield16`, `enum16`

* `uint32` = **MSW puis LSW**

* Lecture cohérente requise pour les valeurs multi-registres

* Registres réservés :



&nbsp; * lecture = `0`

* Aucun effet de bord (lecture passive uniquement)



---



## 5. États globaux



### 5.1 `system_health_status`



| Valeur | Signification |

| -----: | ------------- |

|      0 | OK            |

|      1 | Warning       |

|      2 | Dégradé       |

|      3 | Critique      |

| 4..65535 | réservés    |



---



### 5.2 `system_fault_flags` (bitfield)



|    Bit | Signification                |

| -----: | ---------------------------- |

|      0 | défaut capteur               |

|      1 | défaut acquisition           |

|      2 | défaut mémoire               |

|      3 | défaut SD                    |

|      4 | défaut horloge               |

|      5 | défaut configuration         |

|      6 | défaut interne firmware      |

|      7 | surconsommation              |

|      8 | température hors plage       |

|      9 | communication interne erreur |

| 10..15 | réservés                     |



---



## 6. Résultats autotest



### 6.1 `selftest_status`



| Valeur | Signification  |

| -----: | -------------- |

|      0 | jamais exécuté |

|      1 | en cours       |

|      2 | OK             |

|      3 | échec          |

| 4..65535 | réservés     |



---



## 7. Mapping complet



| Offset | Adresse absolue | Champ                  | Type       | Accès | Description                   |

| -----: | --------------: | ---------------------- | ---------- | ----- | ----------------------------- |

|      0 |            7000 | diag_structure_version | uint16     | RO    | Version structure Bloc 7      |

|      1 |            7001 | system_health_status   | enum16     | RO    | État global                   |

|      2 |            7002 | system_fault_flags     | bitfield16 | RO    | Flags défauts                 |

|      3 |            7003 | last_fault_code        | uint16     | RO    | Dernier défaut détecté        |

|    4-5 |       7004-7005 | last_fault_timestamp   | uint32     | RO    | Timestamp dernier défaut      |

|      6 |            7006 | selftest_status        | enum16     | RO    | État autotest                 |

|      7 |            7007 | selftest_result_code   | uint16     | RO    | Résultat autotest             |

|      8 |            7008 | selftest_detail        | uint16     | RO    | Détail autotest               |

|   9-10 |       7009-7010 | uptime_s               | uint32     | RO    | Temps fonctionnement          |

|     11 |            7011 | reset_cause            | enum16     | RO    | Cause dernier reset           |

|     12 |            7012 | internal_temp_dC       | int16      | RO    | Température interne (déci °C) |

|     13 |            7013 | supply_voltage_mV      | uint16     | RO    | Tension alimentation          |

|  14-15 |       7014-7015 | reserved_7A            | uint16[2]  | RO    | Réservé                       |



---



## 8. Détails des champs critiques



### 8.1 `last_fault_code`



* Code interne du dernier défaut significatif

* Permet corrélation avec logs firmware

* `0` = aucun défaut connu



---



### 8.2 `last_fault_timestamp`



* Timestamp du dernier défaut

* même base temporelle que Bloc 2

* codé en `uint32` (MSW / LSW)



---



### 8.3 `uptime_s`



* Temps de fonctionnement depuis dernier reset

* unité : secondes

* ne doit jamais revenir en arrière sauf reset



---



### 8.4 `reset_cause`



|   Valeur | Signification        |

| -------: | -------------------- |

|        0 | inconnu              |

|        1 | power-on             |

|        2 | reset logiciel       |

|        3 | watchdog             |

|        4 | brown-out            |

|        5 | reset externe        |

|        6 | mise à jour firmware |

| 7..65535 | réservés             |



---



### 8.5 `internal_temp_dC`



* type : `int16`

* unité : **déci °C**

* exemple :



&nbsp; * 253 → 25.3°C

&nbsp; * -50 → -5.0°C



👉 cohérent avec la décision précédente (pas de float + plage négative)



---



### 8.6 `supply_voltage_mV`



* unité : millivolts

* exemple :



&nbsp; * 3300 → 3.3V



---



## 9. Règles d’implémentation



* toute tentative d’écriture sur un registre du Bloc 7 doit être rejetée par une exception Modbus explicite ;
* aucune écriture silencieusement ignorée ou partiellement exécutée n’est autorisée ;
* une tentative d’écriture ne doit modifier aucun registre ni état interne du bloc.

* aucune commande implicite

* mise à jour interne libre (non synchronisée Modbus)

* cohérence des champs critiques :



&nbsp; * timestamp

&nbsp; * uptime

&nbsp; * défauts



---



## 10. Justification



Ce bloc permet :



* diagnostic rapide terrain ;

* intégration supervision simple ;

* corrélation avec logs firmware ;

* robustesse sans dépendance temps réel.



---



## 11. Réserves / extensions futures



* ajout journal des défauts (historique circulaire) ;

* ajout métriques avancées (CPU, RAM, stack) ;

* ajout diagnostic capteur détaillé ;

* extension autotest multi-niveaux.



Les registres réservés permettent ces évolutions.





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

## Code défaut principal

Utilisation métier
1. identifier rapidement le problème dominant
2. orienter le diagnostic terrain

Exemples
1. défaut capteur MEMS
2. erreur carte SD
3. erreur écriture
4. problème alimentation

Contrainte
Ce champ doit refléter le défaut le plus critique à un instant donné.

## Flags défaut détaillés (bitfield)

Utilisation métier
1. fournir une vision complète des anomalies
2. permettre un diagnostic précis

Exemples de défauts
1. capteur MEMS non initialisé
2. erreur communication interne
3. erreur stockage
4. température hors plage
5. tension instable
6. erreur horloge

Contrainte
Les bits doivent être indépendants et cumulables.

## Compteur d’erreurs internes

Utilisation métier
1. suivre la fiabilité globale du capteur
2. détecter une dégradation progressive

Exemple
Un compteur en augmentation indique un problème latent.

Remarque
À interpréter en tendance, pas en valeur absolue.

## Dernier code erreur

Utilisation métier
1. comprendre le dernier incident survenu
2. faciliter l’analyse post-incident

Exemple
Permet d’identifier une erreur ponctuelle non persistante.

## Timestamp dernière erreur

Utilisation métier
1. situer un incident dans le temps
2. corréler avec événements externes

Exemple
Erreur survenue lors d’une montée en charge moteur.

## État capteur MEMS

Utilisation métier
1. vérifier la disponibilité du capteur de mesure
2. détecter un capteur défaillant

Valeurs recommandées
1. non initialisé
2. OK
3. défaut
4. saturation fréquente

Exemple
Un capteur saturé régulièrement indique un mauvais dimensionnement.

## Statistiques saturation

Utilisation métier
1. détecter un mauvais choix de plage MEMS
2. qualifier la qualité des mesures

Exemple
Un taux de saturation élevé rend les données peu exploitables.

## État stockage détaillé

Utilisation métier
1. diagnostiquer finement les problèmes de carte SD
2. distinguer absence, erreur, corruption

Exemples
1. OK
2. absent
3. erreur lecture
4. erreur écriture
5. système de fichiers corrompu

## Statistiques écriture SD

Utilisation métier
1. détecter une usure ou instabilité du support
2. anticiper une panne

Exemple
Un nombre élevé d’erreurs d’écriture indique une carte SD en fin de vie.

## État alimentation détaillé

Utilisation métier
1. qualifier la stabilité de l’alimentation
2. détecter des micro-coupures

Exemples
1. OK
2. instable
3. sous-tension
4. surtension

Exemple
Une alimentation instable peut provoquer des redémarrages ou des pertes de données.

## Historique redémarrages

Utilisation métier
1. détecter une instabilité système
2. identifier des redémarrages anormaux

Exemple
Plusieurs redémarrages en peu de temps indiquent un problème critique.

## Cause dernier redémarrage

Utilisation métier
1. comprendre pourquoi le capteur a redémarré
2. différencier un reset volontaire d’un défaut

Exemples
1. mise sous tension
2. reset logiciel
3. watchdog
4. défaut alimentation

## Charge CPU / activité système

Utilisation métier
1. vérifier que le capteur n’est pas saturé
2. détecter une surcharge logicielle

Exemple
Une charge élevée constante peut indiquer un bug ou un mauvais paramétrage.

## Occupation mémoire

Utilisation métier
1. surveiller les ressources internes
2. détecter des fuites mémoire

Exemple
Une mémoire qui diminue progressivement indique un problème logiciel.

## Version firmware détaillée

Utilisation métier
1. identifier précisément la version embarquée
2. faciliter le support et la maintenance

Exemple
Permet de savoir si un bug connu est présent.

## Mode debug / maintenance

Utilisation métier
1. activer des informations supplémentaires en phase de test
2. limiter les logs en production

Contrainte
Le mode debug ne doit pas impacter la performance en production.

## Approche diagnostic terrain

Le Bloc 7 doit permettre à un technicien de :
1. identifier rapidement la cause d’un problème
2. décider d’une action (redémarrage, remplacement, reconfiguration)
3. éviter un démontage inutile

Objectif
Minimiser les interventions physiques et le temps de diagnostic.

## Lien avec les autres blocs

1. Bloc 1 → état global
2. Bloc 5 → commandes et actions
3. Bloc 6 → impact sur les campagnes

Le Bloc 7 apporte le niveau de détail nécessaire pour comprendre les anomalies détectées ailleurs.
