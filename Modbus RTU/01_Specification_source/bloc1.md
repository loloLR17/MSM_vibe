# TR2 — Bloc 1 : État système

## Mapping Modbus RTU — V1

---

## 1. Objet du bloc

Le Bloc 1 fournit une vision synthétique de l’état global du capteur.

Il permet à la centrale de :
- connaître l’état de fonctionnement général ;
- détecter la présence de défauts ;
- suivre l’activité d’acquisition ;
- évaluer la disponibilité du capteur ;
- disposer d’indicateurs simples pour supervision.

Ce bloc est **strictement en lecture seule (RO)**.

---

## 2. Base d’adressage

- **Bloc 1 base = registre 1000**

---

## 3. Accès

- Bloc entièrement **RO**.
- Toute tentative d’écriture sur un registre du Bloc 1 doit être rejetée par une exception Modbus explicite.
- Aucune écriture silencieusement ignorée ou partiellement exécutée n’est autorisée.
- Une tentative d’écriture ne doit modifier aucun registre ni état interne du bloc.

---

## 4. Conventions générales

- Types autorisés : `uint16`, `int16`, `uint32`, `bitfield16`, `enum16`.
- `uint32` = **MSW puis LSW**.
- Registres réservés : lecture = `0`, écriture interdite.
- Les champs multi-registres doivent être **cohérents lors d’une même lecture**.
- Les états exposés doivent refléter un **snapshot cohérent du système**.

---

## 5. Mapping complet

| Offset | Adresse absolue | Nom | Type | Accès | Description |
|---:|---:|---|---|---|---|
| 0 | 1000 | system_status | enum16 | RO | État global du capteur |
| 1 | 1001 | system_flags | bitfield16 | RO | Drapeaux d’état système |
| 2 | 1002 | fault_flags | bitfield16 | RO | Drapeaux de défauts actifs |
| 3 | 1003 | warning_flags | bitfield16 | RO | Drapeaux d’avertissements |
| 4 | 1004 | uptime_s_msw | uint16 | RO | Temps de fonctionnement (MSW) |
| 5 | 1005 | uptime_s_lsw | uint16 | RO | Temps de fonctionnement (LSW) |
| 6 | 1006 | last_reset_cause | enum16 | RO | Cause du dernier redémarrage |
| 7 | 1007 | internal_temp_dC | int16 | RO | Température interne (déci °C) |
| 8 | 1008 | cpu_load_percent | uint16 | RO | Charge CPU (%) |
| 9 | 1009 | memory_usage_percent | uint16 | RO | Utilisation mémoire (%) |
| 10 | 1010 | storage_status | enum16 | RO | État du stockage |
| 11 | 1011 | storage_usage_percent | uint16 | RO | Occupation stockage (%) |
| 12 | 1012 | acquisition_state | enum16 | RO | État de l’acquisition |
| 13 | 1013 | active_campaign_id_msw | uint16 | RO | ID campagne active (MSW) |
| 14 | 1014 | active_campaign_id_lsw | uint16 | RO | ID campagne active (LSW) |
| 15 | 1015 | error_code | uint16 | RO | Code erreur principal |
| 16 | 1016 | warning_code | uint16 | RO | Code avertissement principal |
| 17 | 1017 | reserved_1 | uint16 | RO | Réservé (0) |
| 18 | 1018 | reserved_2 | uint16 | RO | Réservé (0) |
| 19 | 1019 | reserved_3 | uint16 | RO | Réservé (0) |

---

## 6. Définitions détaillées

### 6.1 `system_status`

Le champ représente l’état global du capteur.

**Le domaine détaillé des codes de `system_status` n’est pas défini dans la V1 et reste À ARBITRER.**

Aucune table de codes issue des compléments métier informatifs ne doit être considérée comme normative tant qu’elle n’a pas été intégrée explicitement à cette section.

---

### 6.2 `last_reset_cause`

| Valeur | Signification |
|---|---|
| 0 | Inconnu |
| 1 | Power-on |
| 2 | Reset logiciel |
| 3 | Watchdog |
| 4 | Brown-out |
| 5 | Reset externe |
| 6 | Mise à jour firmware |
| 7-65535 | Réservé |

---

### 6.6 `storage_status`

| Valeur | Signification |
|---|---|
| 0 | Non disponible |
| 1 | Disponible |
| 2 | Plein |
| 3 | Erreur |
| 4-65535 | Réservé |

---

### 6.7 `acquisition_state`

| Valeur | Signification |
|---|---|
| 0 | Arrêtée |
| 1 | En cours |
| 2 | Pause |
| 3 | Erreur |
| 4-65535 | Réservé |

---

### 6.8 `internal_temp_dC`

- **Type** : `int16`
- **Offset** : `7`
- **Adresse absolue** : `1007`
- **Accès** : `RO`
- **Unité** : `0,1 °C`

Exemples :
- `253` → `25,3 °C`
- `-50` → `-5,0 °C`

---

## 7. Règles d’implémentation

- Les informations doivent représenter un **état cohérent du système**.
- Les flags doivent être **cohérents avec les états globaux**.
- Les compteurs (`uptime`) doivent être **monotones**.
- Les valeurs critiques doivent être mises à jour de manière atomique.
- Les défauts et avertissements doivent être **persistants tant que la condition est présente**.

---

## 8. Justification

Le Bloc 1 fournit une abstraction simple et robuste de l’état du capteur :
- exploitable immédiatement par une centrale ;
- indépendante des détails internes ;
- adaptée à une supervision industrielle.

---

## 9. Réserves / extensions futures

Ajout possible :
- tension alimentation ;
- température externe ;
- état communication RS485 détaillé.

Les registres réservés permettent ces évolutions sans rupture.

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

## État global système

Interprétation métier : ce champ représente l’état global du capteur du point de vue exploitation.

Utilisation métier :
1. déterminer si les données peuvent être utilisées pour l’analyse ;
2. déclencher des alertes opérateur ;
3. prioriser les actions de maintenance.

Valeurs recommandées :
1. 0 = état inconnu / non initialisé ;
2. 1 = nominal ;
3. 2 = dégradé (fonctionnement possible mais altéré) ;
4. 3 = défaut (données non fiables ou indisponibles).

Exemple : un capteur avec carte SD absente peut être en état dégradé.

## État acquisition

Utilisation métier :
1. savoir si le capteur est en train d’enregistrer des données ;
2. vérifier la cohérence avec la configuration attendue.

Valeurs recommandées :
1. 0 = arrêt ;
2. 1 = en cours d’acquisition ;
3. 2 = en attente (armé mais non déclenché).

Exemple : un capteur configuré en mode événementiel peut rester en attente pendant une longue période.

## État stockage (carte SD)

Utilisation métier :
1. vérifier la disponibilité du stockage ;
2. détecter les défauts de carte SD ;
3. anticiper une saturation.

Valeurs recommandées :
1. 0 = inconnu ;
2. 1 = OK ;
3. 2 = absent ;
4. 3 = erreur (lecture/écriture) ;
5. 4 = plein ou quasi plein.

Exemple : une carte SD pleine empêche l’enregistrement de nouvelles campagnes.

## État horloge / synchronisation

Utilisation métier :
1. garantir la cohérence temporelle des données ;
2. valider la corrélation avec d’autres systèmes (navigation, GPS, etc.).

Valeurs recommandées :
1. 0 = non initialisée ;
2. 1 = synchronisée ;
3. 2 = dérive détectée.

Exemple : une horloge non synchronisée rend les données difficilement exploitables en analyse croisée.

## Température interne

Utilisation métier :
1. surveillance des conditions de fonctionnement ;
2. détection de surchauffe ;
3. diagnostic environnemental.

Plage recommandée : -20°C à +70°C (à ajuster selon matériel).

Valeurs particulières : valeur hors plage = condition anormale ; valeur extrême = capteur potentiellement défaillant.

Exemple : un capteur installé près d’un moteur peut atteindre des températures élevées.

## Tension alimentation

Utilisation métier : détection de chute de tension, diagnostic alimentation instable, prévention de perte de données.

Plage recommandée : à définir selon architecture (ex : 3.3V, 5V, 12V).

Exemple : une tension instable peut provoquer des redémarrages intempestifs.

## Compteur d’erreurs

Utilisation métier : suivi de la fiabilité du capteur, détection de dégradation progressive, analyse post-mission.

Exemple : un compteur d’erreurs en augmentation indique un problème latent.

## Flags défaut (bitfield)

Utilisation métier : diagnostic rapide sans lecture détaillée et identification immédiate des défauts critiques.

Exemples de défauts : carte SD absente, erreur capteur MEMS, erreur écriture, température hors plage, alimentation instable.

Contrainte informative : les bits non reconnus doivent être ignorés par la centrale.

## Compteur de redémarrages

Utilisation métier : détection de redémarrages anormaux et diagnostic d’instabilité système.

Exemple : un nombre élevé de redémarrages indique un problème électrique ou logiciel.
