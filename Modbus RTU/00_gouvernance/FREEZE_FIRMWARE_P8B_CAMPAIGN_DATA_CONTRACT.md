# Projet MSM — Capteur de vibration TR2

## Gel firmware P8-B — Campaign Data Contract

Statut : **FW_POLICY / IMPLEMENTATION gelée pour P8**

Cette note ferme les points d'intégration identifiés en P8-A concernant la sémantique de `CampaignDataStore::append()` et la frontière de `checkpoint()`.

Elle ne modifie pas le mapping Modbus RTU V1, ne définit aucun nouveau registre et ne complète pas silencieusement une sémantique protocolaire `NOT_DEFINED V1`.

---

## 1. Références gelées

Le contrat respecte notamment :

- `F-ACQ-01..04` et `F-SUP-01..07` ;
- `G-CAMP-01..03` ;
- `G-DATA-01..02` ;
- `G-REC-01..03` ;
- `G-CMD-01` ;
- la séparation `AcquisitionService` / `SupervisionService` / B3 ;
- la séparation `CampaignService` / `CampaignRepository` / `CampaignDataStore`.

Le `CampaignDataStore` reste un stockage bulk opaque au protocole Modbus.

---

## 2. Autorités et responsabilités

Le modèle retenu est :

```text
VibrationSource
    ↓
VibrationSample
    ↓
AcquisitionService
    ├── statistiques de fenêtre → AcquisitionWindow
    └── échantillon effectivement lu → CampaignService runtime integration
                                      ↓
                              Campaign sample codec
                                      ↓
                            CampaignDataStore::append()
```

`AcquisitionWindow` n'est pas sérialisée comme payload de campagne.

`SupervisionSnapshot` n'est pas sérialisé comme payload de campagne.

B3 n'est jamais utilisé comme source de données de campagne.

Le payload bulk P8 représente les échantillons d'acquisition effectivement retournés avec `TR2_OK` par `VibrationSource::read_sample()`.

---

## 3. Record logique d'échantillon P8

Chaque échantillon persisté est encodé dans un record fixe de **16 octets**.

Le format est indépendant du layout mémoire de `VibrationSample` et ne doit jamais être obtenu par copie brute de la structure C.

| Offset | Taille | Champ | Encodage |
|---:|---:|---|---|
| 0 | 4 | `x_mg` | `int32`, little-endian, complément à deux |
| 4 | 4 | `y_mg` | `int32`, little-endian, complément à deux |
| 8 | 4 | `z_mg` | `int32`, little-endian, complément à deux |
| 12 | 2 | `flags` | `uint16`, little-endian |
| 14 | 2 | `reserved` | `uint16`, valeur `0` |

Bits définis dans `flags` :

```text
bit 0 = sample.valid
bit 1 = sample.saturated
bits 2..15 = 0
```

Un appel `read_sample()` retournant `TR2_OK` produit exactement un record, y compris lorsque `sample.valid == false`.

Un appel `read_sample()` retournant une erreur ne produit aucun record.

Aucun timestamp civil ou monotone n'est ajouté au record P8. La fréquence d'échantillonnage et le contexte historique sont déjà portés par la configuration historique durable de la campagne.

---

## 4. Règle d'append

Pour une campagne ouverte et un DataStore commencé :

```text
read_sample() == TR2_OK
→ encode record 16 bytes
→ CampaignDataStore::append(campaign_id, record, 16)
```

L'append appartient au chemin runtime de campagne ; `AcquisitionService` reste indépendant de l'identité de campagne et du stockage persistant.

Une erreur d'append est une erreur de stockage runtime. Elle interdit de considérer silencieusement l'échantillon comme durable.

La politique de réaction globale du runtime à cette erreur est traitée dans une tranche P8 ultérieure ; P8-B ne transforme pas cette erreur en donnée valide ni en succès de campagne.

---

## 5. Frontière de checkpoint

La frontière logique P8 de `CampaignDataStore::checkpoint()` est **la fin d'une `AcquisitionWindow`**.

Ordre logique :

```text
samples de la fenêtre
→ append de chaque record produit
→ AcquisitionService::end_window()
→ checkpoint(campaign_id)
→ la fenêtre peut ensuite être consommée par SupervisionService
```

Cette règle vaut pour toute fenêtre effectivement terminée par le runtime, qu'elle soit :

- complète ;
- incomplète à la suite d'un arrêt explicite ;
- incomplète à la suite d'une erreur source, dès lors que le runtime peut terminer proprement la fenêtre.

Le checkpoint publie uniquement le préfixe constitué des records dont les `append()` ont réussi avant cette frontière.

Aucun checkpoint par échantillon n'est requis.

Aucune cadence temporelle supplémentaire n'est introduite en P8.

`finish_campaign()` reste la barrière finale de publication du préfixe durable lors de la fermeture de campagne, conformément au contrat existant du DataStore.

---

## 6. Conséquences power-loss

Après une perte d'alimentation :

- seuls les records inclus dans le dernier checkpoint durable ou dans un `finish_campaign()` durable appartiennent au préfixe récupérable ;
- une queue appendue depuis le dernier checkpoint peut être perdue ;
- cette queue n'est jamais reconstruite ou devinée ;
- un chunk partiel/corrompu ne doit pas contaminer le préfixe antérieur démontré valide ;
- une campagne `OPEN` n'est jamais reprise automatiquement.

Ces règles restent conformes à `G-DATA-01`, `G-DATA-02`, `G-REC-01` et `G-REC-02`.

---

## 7. Séparation supervision / historique

La publication d'un `SupervisionSnapshot` et la persistance bulk sont deux responsabilités distinctes.

Le runtime doit néanmoins conserver l'ordre de traitement d'une fenêtre :

```text
fin de fenêtre
→ checkpoint bulk
→ publication supervision de cette fenêtre
```

Ainsi, une supervision publiée ne précède pas la barrière bulk P8 de la fenêtre correspondante.

Ce choix est une `FW_POLICY` de cohérence interne P8 ; il ne crée aucune garantie protocolaire supplémentaire sur B3.

---

## 8. Points explicitement hors périmètre P8-B

P8-B ne définit pas :

- un protocole d'export/download des données bulk ;
- un nouveau mapping Modbus ;
- une compression ;
- une FFT ;
- un format de fichier externe ;
- une politique de rétention média ;
- la réaction système complète à une erreur d'append/checkpoint ;
- la convergence d'un START après OPEN durable mais échec de démarrage de la source ;
- une auto-reprise après reboot.

Ces éléments restent dans leurs tranches ou statuts respectifs.

---

## 9. Invariants P8-B gelés

- **P8B-DATA-01** — un record bulk représente exactement un `VibrationSample` obtenu avec `read_sample() == TR2_OK`.
- **P8B-DATA-02** — le record persistant est encodé explicitement et ne dépend jamais du padding/layout C.
- **P8B-DATA-03** — un sample invalide mais effectivement lu reste historiquement représenté par son record et son flag `valid=0`.
- **P8B-DATA-04** — une erreur de lecture source ne crée aucun record fictif.
- **P8B-DATA-05** — `AcquisitionWindow`, `SupervisionSnapshot` et B3 ne sont pas les payloads bulk de campagne.
- **P8B-CHK-01** — la frontière normale de checkpoint est la fin de fenêtre d'acquisition.
- **P8B-CHK-02** — aucune cadence supplémentaire de checkpoint n'est introduite en P8.
- **P8B-CHK-03** — après power-loss, seule l'autorité de recovery du DataStore définit le préfixe durable.
- **P8B-SEP-01** — `AcquisitionService` ne reçoit ni `campaign_id` ni dépendance vers `CampaignDataStore`.
- **P8B-SEP-02** — `CampaignService`/l'intégration runtime possède le raccordement entre samples et stockage de campagne.
- **P8B-SUP-01** — le checkpoint bulk d'une fenêtre précède sa publication live par `SupervisionService`.

---

## 10. Fermeture des GAP P8-A

Avec ce gel :

```text
G-GAP-04 campaign payload semantic/serialization contract → CLOSED by FW_POLICY P8-B
G-GAP-05 checkpoint cadence                              → CLOSED by FW_POLICY P8-B
```

Les autres GAP P8-A restent ouverts et seront traités par les tranches d'intégration suivantes.
