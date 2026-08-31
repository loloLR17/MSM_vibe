# FT-STR-03 — Fiche de spécification

## Encodage multi-registres et endianness applicative

---

## 1. Identification

- **ID** : FT-STR-03
- **Nom** : Encodage multi-registres
- **Famille parente** : FT-STR
- **Criticité** : P0 (bloquant)

---

## 2. Objectif

Valider que les données multi-registres (notamment `uint32`) sont :

- correctement ordonnées (MSW puis LSW),
- contiguës,
- cohérentes en lecture,
- sans inversion ni ambiguïté.

---

## 3. Périmètre

### Inclus

- Champs `uint32`
- Ordre des mots (MSW → LSW)
- Continuité des registres
- Reconstruction logique des valeurs
- Cohérence en lecture répétée (état stable)
- Cas limites `uint32`
- Détection de faux positifs de reconstruction

### Exclus

- Valeur métier
- Atomicité stricte firmware non spécifiée
- Typage (FT-STR-02)

---

## 4. Références

- Convention : `uint32 = MSW puis LSW`
- Mapping des champs `uint32`

---

## 5. Règles de conformité

Un champ `uint32` est conforme si :

- il occupe exactement 2 registres consécutifs ;
- le registre de poids fort est lu en premier ;
- la reconstruction donne une valeur cohérente ;
- aucune inversion de mots n’est observée ;
- les valeurs limites restent reconstruisibles sans ambiguïté.

---

## 6. Préconditions

- FT-STR-02 validée
- Accès Modbus stable
- État capteur figé

---

## 7. Résultats attendus

- reconstruction correcte des `uint32`
- aucune ambiguïté d’ordre
- comportement stable
- valeurs limites correctement reconstruites

---

## 8. Risques couverts

| Risque | Impact |
|---|---|
| Inversion MSW/LSW | Données fausses |
| Registres non contigus | Valeur corrompue |
| Lecture incohérente | Instabilité |
| Interprétation ambiguë | Intégration impossible |
| Faux positif sur valeur symétrique | Validation erronée |

---

## 9. Cas de figure

### Nominal

- Lecture complète d’un uint32
- Reconstruction correcte

### Limites

- Lecture du MSW seul
- Lecture du LSW seul
- Lecture partielle puis complète
- Valeurs limites `uint32`

### Erreurs

- Inversion volontaire
- Décalage de registre
- Reconstruction incohérente

### Robustesse

- Lecture répétée
- Lecture avec timing variable
- Vérification sur jeux de valeurs extrêmes

---

## 10. Points critiques

### Endianness

Erreur la plus critique du protocole.

### Faux positifs

Certaines valeurs symétriques masquent une inversion.

### Cohérence temporelle

MSW et LSW doivent représenter le même instant.

---

## 11. Ambiguïtés

- Atomicité garantie ou non ?
- Valeurs de test injectables ?
- Comportement en lecture concurrente ?
- Mode de forçage des valeurs limites disponible ou non ?

---

## 12. Critères de réussite

- 100% des uint32 correctement reconstruits
- aucune inversion détectée
- comportement stable
- jeux de valeurs limites correctement traités

---

## 13. Critères d’échec

- inversion MSW/LSW
- incohérence de reconstruction
- discontinuité
- faux positif non détecté

---

## 14. Anomalies

| Type | Exemple |
|---|---|
| BLOQUANTE | inversion |
| MAJEURE | registre non contigu |
| MAJEURE | reconstruction limite incorrecte |
| MINEURE | instabilité faible |
| SPEC | ambiguïté doc |

---

## 15. Dépendances

### Amont

- FT-STR-02

### Aval

- FT-BLK
- FT-INT

---

## 16. Ordre d’exécution

1. Identifier champs uint32
2. Lire MSW + LSW
3. Reconstituer
4. Tester inversion
5. Tester répétabilité
6. Tester valeurs limites
7. Tester faux positifs

---

## 17. Livrables

- tableau des uint32
- reconstruction validée
- traces lecture
- résultats des jeux de valeurs limites

---

## 18. Tests de limites et robustesse des types

### Objectif

Valider que les champs multi-registres (`uint32`) sont correctement reconstruits et interprétés sur toute leur plage de définition.

### Cas à couvrir

#### Valeurs limites

- `0x00000000`
- `0x00000001`
- `0x7FFFFFFF`
- `0x80000000`
- `0xFFFFFFFF`

### Vérifications

Pour chaque valeur :

- reconstruction correcte `MSW << 16 | LSW`
- absence d’inversion MSW/LSW
- absence de troncature
- cohérence sur lectures répétées

### Détection d’erreurs

- inversion MSW/LSW
- overflow implicite
- valeur incohérente
- variation entre lectures

### Critères d’acceptation

- valeur reconstruite strictement conforme
- aucune variation en état stable
- aucune ambiguïté de reconstruction

### Note d’applicabilité

Ces tests peuvent nécessiter :
- un banc de simulation,
- un mode debug firmware,
- ou un mécanisme de forçage contrôlé des valeurs.
