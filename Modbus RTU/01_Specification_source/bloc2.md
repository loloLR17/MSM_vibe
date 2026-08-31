# TR2 — Bloc 2 : Temps



## Mapping Modbus RTU — V1



---



## 1. Objet du bloc



Le Bloc 2 gère la référence temporelle du capteur.



Il permet à la centrale de :



* lire l’heure courante du capteur ;

* préparer une synchronisation temporelle ;

* vérifier la validité de la base de temps ;

* connaître l’état de synchronisation.



Ce bloc supporte une logique **préparé / appliqué** cohérente avec l’architecture globale.



---



## 2. Base d’adressage



* **Bloc 2 base = registre 2000**



---



## 3. Accès



* Lecture globale : **RO**

* Zone de préparation : **RW**

* Application effective uniquement via **Bloc 5 (commande)**



---



## 4. Conventions générales



* Types autorisés : `uint16`, `int16`, `uint32`, `enum16`, `bitfield16`

* `uint32` = **MSW puis LSW**

* Unité de temps :



&nbsp; * secondes depuis **Epoch TR2**

* Registres réservés :



&nbsp; * lecture = `0`

&nbsp; * écriture interdite

* Les champs multi-registres doivent être **cohérents lors d’une même lecture**

* L’heure courante doit être issue d’un **snapshot cohérent**



---



## 5. Mapping complet



| Offset | Adresse absolue | Nom                   | Type       | Accès | Description                             |

| ------ | --------------- | --------------------- | ---------- | ----- | --------------------------------------- |

| 0      | 2000            | time_status           | enum16     | RO    | État global de la base de temps         |

| 1      | 2001            | time_flags            | bitfield16 | RO    | Drapeaux de validité et synchronisation |

| 2      | 2002            | current_time_msw      | uint16     | RO    | Temps courant (MSW)                     |

| 3      | 2003            | current_time_lsw      | uint16     | RO    | Temps courant (LSW)                     |

| 4      | 2004            | last_sync_time_msw    | uint16     | RO    | Dernière synchronisation (MSW)          |

| 5      | 2005            | last_sync_time_lsw    | uint16     | RO    | Dernière synchronisation (LSW)          |

| 6      | 2006            | time_since_sync_s_msw | uint16     | RO    | Temps écoulé depuis dernière sync (MSW) |

| 7      | 2007            | time_since_sync_s_lsw | uint16     | RO    | Temps écoulé depuis dernière sync (LSW) |

| 8      | 2008            | prepared_time_msw     | uint16     | RW    | Temps préparé (MSW)                     |

| 9      | 2009            | prepared_time_lsw     | uint16     | RW    | Temps préparé (LSW)                     |

| 10     | 2010            | prepared_time_status  | enum16     | RO    | État du temps préparé                   |

| 11     | 2011            | time_accuracy_ms      | uint16     | RO    | Précision estimée de l’horloge (ms)     |

| 12     | 2012            | drift_ppm             | int16      | RO    | Dérive estimée (ppm)                    |

| 13     | 2013            | sync_source           | enum16     | RO    | Source de synchronisation               |

| 14     | 2014            | reserved_1            | uint16     | RO    | Réservé (0)                             |

| 15     | 2015            | reserved_2            | uint16     | RO    | Réservé (0)                             |



---



## 6. Définitions détaillées



### 6.1 `time_status`



| Valeur  | Signification                |

| ------- | ---------------------------- |

| 0       | Non initialisé               |

| 1       | Temps invalide               |

| 2       | Temps valide non synchronisé |

| 3       | Temps synchronisé            |

| 4       | Temps dégradé                |

| 5-65535 | Réservé                      |



---



### 6.2 `time_flags`



| Bit  | Signification             |

| ---- | ------------------------- |

| 0    | Temps valide              |

| 1    | Synchronisation effectuée |

| 2    | Synchronisation en cours  |

| 3    | Temps préparé disponible  |

| 4    | Source de temps fiable    |

| 5    | Dérive hors tolérance     |

| 6    | Synchronisation requise   |

| 7    | Horloge stable            |

| 8-15 | Réservés                  |



---



### 6.3 `prepared_time_status`



| Valeur  | Signification            |

| ------- | ------------------------ |

| 0       | Aucun temps préparé      |

| 1       | Temps préparé disponible |

| 2       | Temps appliqué           |

| 3       | Erreur de préparation    |

| 4-65535 | Réservé                  |



---



### 6.4 `sync_source`



| Valeur  | Signification |

| ------- | ------------- |

| 0       | Inconnue      |

| 1       | Centrale      |

| 2       | GPS           |

| 3       | RTC interne   |

| 4       | Réseau        |

| 5-65535 | Réservé       |



---



## 7. Règles d’implémentation



* L’écriture de `prepared_time` ne doit **pas modifier immédiatement l’horloge**

* L’application du temps doit passer par :



&nbsp; * **Bloc 5 — commande "synchroniser heure"**

* `current_time` doit être :



&nbsp; * monotone (sauf resynchronisation)

&nbsp; * cohérent entre MSW et LSW

* `last_sync_time` doit être mis à jour uniquement lors d’une synchronisation effective

* `time_since_sync` doit être dérivé de manière cohérente

* En cas de temps invalide :



&nbsp; * les flags doivent refléter clairement l’état

* Les transitions d’état doivent être explicites (pas d’ambiguïté)



---



## 8. Justification



Le découplage :



* préparation (Bloc 2)

* application (Bloc 5)



permet :



* robustesse Modbus ;

* absence d’effet implicite ;

* traçabilité des opérations de synchronisation.



L’ajout de :



* dérive (`ppm`)

* précision (`ms`)

* source



permet une exploitation avancée côté centrale.



---



## 9. Réserves / extensions futures



* Ajout possible :



&nbsp; * fuseau horaire (si besoin spécifique)

&nbsp; * gestion UTC offset

&nbsp; * synchronisation périodique automatique

* Les registres réservés permettent ces évolutions sans rupture





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

## Temps courant (timestamp interne)

Interprétation métier
Ce champ représente l’horloge interne du capteur utilisée pour dater les acquisitions.

Utilisation métier
1. horodatage des mesures vibratoires
2. reconstruction temporelle des campagnes
3. corrélation avec d’autres capteurs

Contrainte
Le timestamp doit être monotone et cohérent sur toute la durée d’une campagne.

Attention
Une dérive importante rend les données difficiles à exploiter en analyse croisée.

## État synchronisation

Utilisation métier
1. déterminer si l’horloge est fiable
2. qualifier la qualité des données temporelles

Valeurs recommandées
1. 0 = non initialisée
2. 1 = synchronisée récemment
3. 2 = synchronisée mais dérive possible
4. 3 = non fiable

Exemple
Après un redémarrage sans resynchronisation, l’état doit être "non initialisée".

## Source de synchronisation

Utilisation métier
1. identifier l’origine du temps
2. qualifier la précision attendue

Valeurs recommandées
1. 0 = inconnue
2. 1 = centrale (Modbus)
3. 2 = GPS indirect via centrale
4. 3 = horloge interne seule

Exemple
Une synchronisation via GPS (indirecte) permet une corrélation globale des données navire.

## Timestamp dernière synchronisation

Utilisation métier
1. évaluer la fraîcheur de la synchronisation
2. détecter un capteur isolé

Exemple
Un capteur non resynchronisé depuis plusieurs jours peut dériver significativement.

## Dérive horloge estimée

Utilisation métier
1. qualifier la précision temporelle
2. corriger éventuellement les données côté centrale

Unité recommandée
ppm (parties par million) ou ms/heure

Exemple
Une dérive de 20 ppm correspond à ~1.7 secondes par jour.

Contrainte
Ce champ peut être estimatif.

## Précision horloge

Utilisation métier
1. évaluer la qualité intrinsèque de l’horloge
2. adapter les algorithmes d’analyse

Exemple
Une horloge basse précision impose des fenêtres d’analyse plus larges.

## Compteur de synchronisations

Utilisation métier
1. vérifier que le capteur est régulièrement synchronisé
2. détecter un problème de communication avec la centrale

Exemple
Un compteur constant indique une absence de resynchronisation.

## Mode de synchronisation

Utilisation métier
1. comprendre le comportement du capteur
2. vérifier la configuration

Valeurs recommandées
1. 0 = manuel uniquement
2. 1 = automatique périodique
3. 2 = à la connexion centrale

Exemple
Un mode automatique est recommandé pour limiter la dérive.

## Validité du temps

Utilisation métier
1. indiquer si les timestamps sont exploitables
2. filtrer les données côté centrale

Valeurs recommandées
1. 0 = invalide
2. 1 = valide

Exemple
Après un redémarrage sans synchronisation, le temps doit être considéré invalide.

## Corrélation avec navigation

Le capteur ne dispose pas de GPS embarqué.
La référence temporelle globale du système est fournie par la centrale, elle-même synchronisée aux données de navigation (ex : NMEA).

Conséquence
La qualité de la synchronisation impacte directement :
1. la corrélation avec la vitesse du navire
2. l’estimation des conditions (profondeur, squat, charge)
3. l’analyse des phénomènes vibratoires dépendants du régime

Recommandation
Une resynchronisation régulière est fortement recommandée.
