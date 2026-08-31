# TR2 — Bloc 3 : Supervision vibratoire

## Mapping Modbus RTU — V1



---



## 1. Objet du bloc



Le Bloc 3 fournit à la centrale une vision synthétique et immédiatement exploitable de l’état vibratoire courant du capteur.



Il n’expose pas :

- les données brutes ;

- la FFT en V1.



La supervision vibratoire V1 repose sur :

- des **indicateurs globaux** en **norme vectorielle d’accélération** ;

- des **indicateurs par axe** X / Y / Z conservés comme informations complémentaires ;

- des **états de validité, alarmes et fraîcheur** ;

- des **métadonnées de cohérence de calcul**.



---



## 2. Base d’adressage



- **Bloc 3 base = registre 3000**



---





## 3. Accès

- Bloc entièrement **RO**
- Toute tentative d’écriture sur un registre du Bloc 3 doit être rejetée par une exception Modbus explicite.
- Aucune écriture silencieusement ignorée ou partiellement exécutée n’est autorisée.
- Une tentative d’écriture ne doit modifier aucun registre ni état interne du bloc.



---



## 4. Conventions générales



- Les champs `uint32` de ce bloc doivent être exposés de manière cohérente lors d’une même lecture Modbus, conformément à la règle générale d’atomicité logique du protocole.

- Accès du Bloc 3 : **lecture seule** côté centrale.

- Les valeurs RMS et crête sont exprimées en **milli-g**.

- Les valeurs globales sont calculées sur la **norme vectorielle**.

- Les valeurs par axe sont fournies à titre complémentaire.

- Les seuils de décision sont configurés dans le **Bloc 4**.

- Le Bloc 3 ne contient que les **résultats de supervision**.



---



## 5. Convention de calcul retenue



### 5.1 Grandeurs globales



- **RMS global** : RMS de la norme vectorielle d’accélération sur la fenêtre de calcul

- **Crête globale** : maximum de la norme vectorielle d’accélération observé sur la fenêtre de calcul



### 5.2 Grandeurs par axe



- **RMS X/Y/Z** : RMS de l’accélération sur chaque axe

- **Crête X/Y/Z** : valeur absolue maximale observée sur chaque axe sur la fenêtre de calcul



### 5.3 Convention normative V1 des grandeurs vibratoires



En version V1, toutes les grandeurs vibratoires exposées par le protocole de supervision sont des **accélérations** exprimées en **mg**.



Cela s’applique à :

- toutes les valeurs RMS ;

- toutes les valeurs crête ;

- les valeurs globales ;

- les valeurs par axe ;

- les seuils et hystérésis associés configurés dans le Bloc 4.



Les grandeurs globales sont calculées à partir de la **norme vectorielle d’accélération**.  

Les grandeurs par axe sont calculées à partir de l’accélération mesurée sur chaque axe.



Aucune valeur en vitesse (`mm/s`) n’est exposée dans la version V1 du protocole.



Toute évolution future vers une exposition en vitesse devra faire l’objet :

- d’une nouvelle spécification explicite ;

- de nouveaux noms de champs ;

- et, si nécessaire, d’une nouvelle version de mapping.



### 5.4 Règle d’application des seuils



Les seuils appliqués aux indicateurs du Bloc 3 sont ceux de la configuration active au moment du calcul.



Un changement de configuration :

- ne rétroagit pas sur les valeurs déjà calculées ;

- prend effet à partir de la prochaine fenêtre de calcul validée.



---



## 6. Politique de validité



Les valeurs du Bloc 3 peuvent être présentes même si elles ne sont plus fraîches.



La confiance dans les valeurs s’évalue via :

- le **statut global de supervision** ;

- le **bitfield de validité** ;

- l’**âge de la dernière valeur valide** ;

- l’**horodatage de dernière mise à jour**.



En cas d’indisponibilité temporaire, le capteur peut conserver la dernière valeur calculée, mais doit positionner les bits et statuts appropriés.



---



## 7. Mapping complet



| Offset | Adresse absolue | Nom symbolique | Type | Accès | Unité / format | Description | Justification |

|---:|---:|---|---|---|---|---|---|

| 0 | 3000 | B3_STATUS_GLOBAL | enum16 | RO | enum | Statut global de supervision vibratoire | Permet une lecture synthétique immédiate |

| 1 | 3001 | B3_VALIDITY_FLAGS | bitfield16 | RO | bits | Drapeaux détaillés de validité / fraîcheur / cohérence | Permet de qualifier la confiance dans les mesures |

| 2 | 3002 | B3_ALARM_FLAGS | bitfield16 | RO | bits | Drapeaux détaillés d’alarme vibratoire | Lisible directement par la centrale |

| 3 | 3003 | B3_SEVERITY_GLOBAL | enum16 | RO | enum | Niveau de sévérité global courant | Permet affichage / filtrage simple |

| 4-5 | 3004-3005 | B3_LAST_UPDATE_TR2 | uint32 | RO | s | Horodatage UTC de la dernière mise à jour valide | Indispensable pour dater les valeurs |

| 6-7 | 3006-3007 | B3_VALUE_AGE_MS | uint32 | RO | ms | Âge de la dernière valeur valide | Permet de distinguer frais / périmé |

| 8-9 | 3008-3009 | B3_CALC_SEQUENCE | uint32 | RO | compteur | Compteur monotone de calcul | Utile pour détecter un blocage de mise à jour |

| 10-11 | 3010-3011 | B3_WINDOW_DURATION_MS | uint32 | RO | ms | Durée de la fenêtre de calcul associée aux valeurs | Permet d’interpréter correctement les indicateurs |

| 12-13 | 3012-3013 | B3_VALID_SAMPLE_COUNT | uint32 | RO | échantillons | Nombre d’échantillons valides utilisés dans la fenêtre | Contrôle de cohérence de calcul |

| 14-15 | 3014-3015 | B3_RMS_GLOBAL_MG | uint32 | RO | mg | RMS global en norme vectorielle | Indicateur principal inter-capteurs |

| 16-17 | 3016-3017 | B3_PEAK_GLOBAL_MG | uint32 | RO | mg | Crête globale en norme vectorielle | Indicateur principal inter-capteurs |

| 18-19 | 3018-3019 | B3_RMS_X_MG | uint32 | RO | mg | RMS axe X | Analyse directionnelle pour un capteur donné |

| 20-21 | 3020-3021 | B3_RMS_Y_MG | uint32 | RO | mg | RMS axe Y | Analyse directionnelle pour un capteur donné |

| 22-23 | 3022-3023 | B3_RMS_Z_MG | uint32 | RO | mg | RMS axe Z | Analyse directionnelle pour un capteur donné |

| 24-25 | 3024-3025 | B3_PEAK_X_MG | uint32 | RO | mg | Crête axe X | Diagnostic complémentaire |

| 26-27 | 3026-3027 | B3_PEAK_Y_MG | uint32 | RO | mg | Crête axe Y | Diagnostic complémentaire |

| 28-29 | 3028-3029 | B3_PEAK_Z_MG | uint32 | RO | mg | Crête axe Z | Diagnostic complémentaire |

| 30 | 3030 | B3_DOMINANT_AXIS | enum16 | RO | enum | Axe dominant sur la dernière fenêtre | Lecture directionnelle synthétique |

| 31 | 3031 | B3_EXCEED_GLOBAL | enum16 | RO | 0/1 | Dépassement global courant | Résultat simple d’évaluation de seuil |

| 32 | 3032 | B3_EXCEED_X | enum16 | RO | 0/1 | Dépassement courant axe X | Diagnostic par axe |

| 33 | 3033 | B3_EXCEED_Y | enum16 | RO | 0/1 | Dépassement courant axe Y | Diagnostic par axe |

| 34 | 3034 | B3_EXCEED_Z | enum16 | RO | 0/1 | Dépassement courant axe Z | Diagnostic par axe |

| 35 | 3035 | B3_ALARM_LATCHED | enum16 | RO | 0/1 | Présence d’une alarme mémorisée | Comportement industriel robuste |

| 36-37 | 3036-3037 | B3_EXCEED_COUNT | uint32 | RO | compteur | Compteur total de dépassements détectés | Utile pour maintenance et supervision |

| 38-39 | 3038-3039 | B3_ALARM_COUNT | uint32 | RO | compteur | Compteur total d’alarmes générées | Historique synthétique sans journal détaillé |

| 40-47 | 3040-3047 | B3_RESERVED_0 | uint16[8] | RO | 0 | Réserve d’extension future | Permet évolution sans casser le bloc |



---



## 8. Définitions détaillées



### 8.1 `B3_STATUS_GLOBAL`



| Valeur | Signification |

|---:|---|

| 0 | Non initialisé |

| 1 | Supervision inactive |

| 2 | Valeurs en cours d’acquisition / calcul non disponible |

| 3 | Valeurs valides |

| 4 | Valeurs dégradées mais exploitables |

| 5 | Valeurs invalides |

| 6 à 65535 | Réservé |



### 8.2 `B3_VALIDITY_FLAGS`



| Bit | Nom | Sens à 1 |

|---:|---|---|

| 0 | SUPERVISION_ENABLED | Supervision vibratoire activée |

| 1 | VALUES_VALID | Valeurs courantes valides |

| 2 | VALUES_FRESH | Valeurs considérées fraîches |

| 3 | WINDOW_COMPLETE | Fenêtre de calcul complète |

| 4 | SAMPLE_COUNT_VALID | Nombre d’échantillons suffisant |

| 5 | SENSOR_NOT_SATURATED | Pas de saturation détectée |

| 6 | TIMEBASE_VALID | Base de temps valide |

| 7 | CONFIG_VALID | Configuration de supervision cohérente |

| 8 | CAMPAIGN_ACTIVE | Campagne / acquisition active |

| 9 | LAST_VALUE_HELD | Dernière valeur conservée sans recalcul récent |

| 10 | DATA_DEGRADED | Données dégradées mais potentiellement utilisables |

| 11 | CALC_ERROR | Erreur de calcul détectée |

| 12 | ACQ_STOPPED | Acquisition arrêtée |

| 13 | RESERVED | Réservé |

| 14 | RESERVED | Réservé |

| 15 | RESERVED | Réservé |



**Note de conception** : pour les bits de type `*_VALID` ou `*_ENABLED`, la valeur `1` signifie explicitement un état favorable afin de simplifier l’interprétation côté centrale.



### 8.3 `B3_ALARM_FLAGS`



| Bit | Nom | Sens à 1 |

|---:|---|---|

| 0 | ALARM_GLOBAL_ACTIVE | Alarme globale active |

| 1 | ALARM_X_ACTIVE | Alarme axe X active |

| 2 | ALARM_Y_ACTIVE | Alarme axe Y active |

| 3 | ALARM_Z_ACTIVE | Alarme axe Z active |

| 4 | EXCEED_GLOBAL_ACTIVE | Dépassement global courant |

| 5 | EXCEED_X_ACTIVE | Dépassement courant axe X |

| 6 | EXCEED_Y_ACTIVE | Dépassement courant axe Y |

| 7 | EXCEED_Z_ACTIVE | Dépassement courant axe Z |

| 8 | ALARM_LATCHED_PRESENT | Alarme mémorisée présente |

| 9 | RESERVED | Réservé |

| 10 | RESERVED | Réservé |

| 11 | RESERVED | Réservé |

| 12 | RESERVED | Réservé |

| 13 | RESERVED | Réservé |

| 14 | RESERVED | Réservé |

| 15 | RESERVED | Réservé |



### 8.4 `B3_SEVERITY_GLOBAL`



| Valeur | Signification |

|---:|---|

| 0 | Non applicable |

| 1 | Normal |

| 2 | Information |

| 3 | Avertissement |

| 4 | Alarme |

| 5 | Critique |

| 6 à 65535 | Réservé |



### 8.5 `B3_DOMINANT_AXIS`



| Valeur | Signification |

|---:|---|

| 0 | Non déterminé |

| 1 | X |

| 2 | Y |

| 3 | Z |

| 4 | Ex aequo / indéterminé |

| 5 à 65535 | Réservé |



### 8.6 `B3_EXCEED_*`



| Valeur | Signification |

|---:|---|

| 0 | Non |

| 1 | Oui |

| 2 à 65535 | Réservé |



### 8.7 `B3_ALARM_LATCHED`



| Valeur | Signification |

|---:|---|

| 0 | Non |

| 1 | Oui |

| 2 à 65535 | Réservé |



---



## 9. Règles d’implémentation



- Les registres du Bloc 3 doivent être mis à jour de manière cohérente, idéalement après validation complète d’une nouvelle fenêtre.

- Il est recommandé d’éviter qu’une centrale lise un mélange d’anciennes et de nouvelles valeurs au sein d’un même cycle de mise à jour.

- Si nécessaire, l’implémentation pourra utiliser une structure miroir ou un mécanisme de copie atomique.

- Les compteurs `B3_EXCEED_COUNT` et `B3_ALARM_COUNT` sont monotones et peuvent saturer à `0xFFFFFFFF`.

- Les registres réservés doivent être maintenus à `0`.

- Le firmware doit exposer les valeurs du Bloc 3 à partir d’un snapshot cohérent correspondant à une fenêtre de calcul validée.



En particulier :

- les champs `uint32` doivent être cohérents registre haut / registre bas ;

- il convient d’éviter qu’une centrale lise un mélange de valeurs issues de deux cycles de calcul différents ;

- l’usage d’une structure miroir ou d’une copie atomique logique est recommandé.



---



## 10. Justification d’architecture



### 10.1 Pourquoi conserver les valeurs globales

Les capteurs n’étant pas nécessairement orientés de manière identique, les valeurs globales sont la référence principale de supervision et de comparaison inter-capteurs.



### 10.2 Pourquoi conserver aussi les axes

Les valeurs par axe restent utiles pour :

- comparer un même capteur d’une campagne à l’autre ;

- observer une dérive directionnelle ;

- aider au diagnostic local.



### 10.3 Pourquoi utiliser `uint32` pour les niveaux

Même si les valeurs en mg pourraient souvent tenir en `uint16`, l’usage systématique de `uint32` :

- homogénéise l’implémentation ;

- simplifie les évolutions futures ;

- évite les limitations ultérieures sur les compteurs et grandeurs calculées.



### 10.4 Pourquoi exposer âge + timestamp + séquence

Ces trois informations sont complémentaires :

- timestamp : date absolue ;

- âge : fraîcheur instantanée ;

- séquence : vérification de l’évolution effective des calculs.



C’est plus robuste qu’un seul indicateur temporel.



---



## 11. Réserves / extensions futures



Le présent mapping V1 reste compatible avec une évolution future vers d’autres grandeurs vibratoires, notamment en vitesse `mm/s`, mais une telle évolution nécessitera :

- une spécification explicite ;

- des noms de champs dédiés ;

- une révision normative du mapping avant toute mise en œuvre.



---



## 12. Conclusion



Le présent mapping V1 du Bloc 3 est cohérent avec :

- une architecture store-and-forward ;

- une supervision temps réel légère ;

- des indicateurs simples précompilés ;

- une lisibilité industrielle immédiate.



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

## RMS vibratoire (global ou par axe)

Interprétation métier
Ce champ représente le niveau vibratoire global du capteur sur une période courte.

Utilisation métier
1. détection rapide d’une dérive ou d’un défaut
2. surveillance continue d’un équipement critique
3. déclenchement d’alertes

Unité recommandée
mg RMS (convention normative V1 du protocole)

Remarque
Même si le mm/s RMS est une grandeur courante en vibration machine, la version V1 du protocole TR2 expose uniquement des accélérations en mg.
Toute comparaison ou tout seuil doit donc être interprété dans cette unité.

Exemple
Une augmentation progressive du RMS peut indiquer une usure de roulement.

Contrainte
Le calcul doit être stable et reproductible.

## Valeur crête (peak)

Utilisation métier
1. détection de chocs ou événements transitoires
2. identification de défauts ponctuels

Exemple
Un pic élevé peut correspondre à un choc mécanique ou une cavitation.

## Bande fréquentielle dominante

Utilisation métier
1. identification grossière du type de défaut
2. corrélation avec régime machine

Exemple
1. basse fréquence → déséquilibre
2. fréquence élevée → défaut roulement

Contrainte
Cette information est indicative, pas une analyse FFT complète.

## Fréquence dominante

Utilisation métier
1. corrélation avec vitesse de rotation
2. identification de phénomènes périodiques

Exemple
Une fréquence dominante proche de la vitesse moteur indique un déséquilibre.

## Indicateur de dépassement seuil

Utilisation métier
1. détection immédiate d’une condition anormale
2. déclenchement d’alarme

Valeurs recommandées
1. 0 = normal
2. 1 = seuil dépassé

Exemple
Dépassement RMS → alerte maintenance.

## Niveau d’alarme vibratoire

Utilisation métier
1. classification de la gravité
2. priorisation des actions

Valeurs recommandées
1. 0 = normal
2. 1 = alerte
3. 2 = alarme
4. 3 = critique

Exemple
Niveau critique → arrêt possible de la machine.

## Compteur d’événements vibratoires

Utilisation métier
1. suivi de l’activité vibratoire anormale
2. détection de phénomènes intermittents

Exemple
Un compteur élevé indique un environnement instable ou dégradé.

## Fenêtre d’analyse utilisée

Utilisation métier
1. compréhension de la base de calcul
2. cohérence entre capteurs

Exemple
Une fenêtre courte donne une mesure plus réactive mais plus bruitée.

## Validité des données vibratoires

Utilisation métier
1. vérifier que la mesure est exploitable
2. éviter les fausses interprétations

Valeurs recommandées
1. 0 = invalide
2. 1 = valide

Exemple
Capteur mal initialisé → données invalides.

## Positionnement dans l’architecture

Le Bloc 3 fournit une supervision temps réel simplifiée.

Il ne remplace pas :
1. l’analyse complète basée sur les données brutes stockées (Bloc 6)
2. les traitements avancés (FFT, spectres, diagnostics)

Utilisation recommandée
1. détection rapide d’anomalies
2. surveillance continue
3. déclenchement d’acquisition ou d’inspection

Limitation
Les indicateurs fournis sont synthétiques et ne permettent pas un diagnostic complet.
