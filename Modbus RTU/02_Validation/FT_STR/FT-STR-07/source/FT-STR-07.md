# FT-STR-07 — Fiche de spécification

## Stabilité d’image, déterminisme et cohérence multi-registres

---

## 1. Identification

- **ID** : FT-STR-07
- **Nom** : Stabilité d’image
- **Famille parente** : FT-STR
- **Criticité** : P0 (bloquant)

---

## 2. Objectif

Valider que, dans un état stable du capteur :

- les lectures sont reproductibles,
- les champs structurels ne varient pas,
- l’image registre est déterministe,
- les champs multi-registres restent cohérents,
- aucune incohérence de reconstruction `uint32` n’est observable.

---

## 3. Périmètre

### Inclus

- répétabilité des lectures
- stabilité des champs statiques
- stabilité des registres réservés
- stabilité des champs ASCII fixes
- cohérence des champs multi-registres
- absence de bruit mémoire
- absence d’effet de bord entre champs
- comparaison snapshot à snapshot
- alternance de lectures unitaires et de lectures bloc

### Exclus

- dynamique métier
- événements
- transitions fonctionnelles volontaires
- performance bus
- contention multi-maître

---

## 4. Références

- état stable défini
- mapping
- convention `uint32 = MSW puis LSW`

---

## 5. Règles

Une image est conforme si :

- deux lectures successives d’un même champ structurel stable sont identiques ;
- deux snapshots successifs d’un même bloc stable sont identiques ;
- aucun champ réservé ne varie ;
- aucun champ ASCII fixe ne varie hors cas explicitement prévu ;
- toute reconstruction `uint32` reste identique sur les lectures répétées en état stable ;
- aucune incohérence de type lecture croisée `MSW(t0) + LSW(t1)` n’est observable en lecture passive sur état stable.

---

## 6. Préconditions

- capteur figé
- aucune acquisition active
- environnement stable
- FT-STR-03 validée
- FT-STR-05 validée

---

## 7. Résultats attendus

- image identique à répétition
- aucune fluctuation
- aucune incohérence multi-registres
- aucune instabilité localisée

---

## 8. Risques

| Risque | Impact |
|---|---|
| instabilité | non fiabilité |
| bruit mémoire | erreurs |
| incohérence multi-registre | corruption |
| effet de bord | non déterminisme |
| lecture croisée MSW/LSW | valeur fausse |

---

## 9. Cas

### Nominal

- lecture répétée identique d’un champ statique
- lecture répétée identique d’un bloc complet
- reconstruction répétée identique d’un `uint32`

### Limites

- lecture rapide répétée
- lecture espacée
- alternance champ / bloc / champ
- alternance champ A / champ B / champ A

### Erreurs

- variation inattendue
- instabilité localisée
- mismatch `MSW/LSW`
- variation d’un réservé

### Robustesse

- série longue de lectures
- snapshots comparés en séquence
- répétition avec tailles de lecture différentes

---

## 10. Points critiques

- rafraîchissement mémoire
- concurrence lecture / mise à jour interne
- latence
- champs multi-registres exposés comme valeur logique unique
- faux positifs sur valeurs peu variées

---

## 11. Ambiguïtés

- quels champs sont explicitement autorisés à varier en état stable ?
- atomicité firmware garantie ou non ?
- politique de rafraîchissement de l’image registre ?

---

## 12. Réussite

- stabilité totale des champs structurels
- stabilité totale des snapshots de blocs stables
- 0 incohérence de reconstruction `uint32`
- 0 variation sur champs réservés

---

## 13. Échec

- variation non expliquée
- incohérence multi-registres
- instabilité locale ou globale
- effet de bord structurel

---

## 14. Anomalies

| Type | Exemple |
|---|---|
| BLOQUANTE | instabilité globale |
| MAJEURE | incohérence `uint32` observable |
| MAJEURE | variation d’un réservé |
| SPEC | champ supposé stable non défini comme tel |

---

## 15. Dépendances

### Amont

- FT-STR-03
- FT-STR-05

### Aval

- FT-INT
- FT-BLK

---

## 16. Ordre

1. lecture répétée champ unitaire
2. lecture répétée champ réservé
3. lecture répétée champ ASCII
4. lecture répétée `uint32`
5. snapshots bloc
6. alternance de lectures
7. détection d’instabilité localisée
8. validation globale

---

## 17. Livrables

- logs lecture
- comparaison champ à champ
- comparaison snapshot à snapshot
- liste des incohérences multi-registres

---

## 18. Maturité

- image déterministe
- reproductibilité totale
- aucune incohérence observable sur champs logiques multi-registres
