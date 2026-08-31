# FT-ACC-01 — Fiche de spécification

## Lecture des zones exposées

---

## 1. Identification

- **ID** : FT-ACC-01
- **Nom** : Lecture des zones exposées
- **Famille parente** : FT-ACC
- **Criticité** : P0 (bloquant)

---

## 2. Objectif

Valider que toute zone de registres déclarée lisible par le référentiel de mapping est effectivement accessible en lecture via l’interface Modbus, dans les conditions nominales prévues par le protocole.

---

## 3. Périmètre

### Inclus

- lecture d’un registre unique documenté lisible ;
- lecture d’une plage contiguë documentée lisible ;
- lecture de champs `uint16` ;
- lecture de champs `uint32` sur leurs 2 registres ;
- lecture de champs ASCII fixes sur leur longueur documentée ;
- lecture de sous-plages incluses dans une zone lisible ;
- lecture répétée d’une même zone lisible, à état simulé inchangé ;
- lecture sur plusieurs blocs exposés, lors de l’instanciation.

### Exclus

- conformité de typage binaire détaillée ;
- ordre MSW/LSW ;
- conformité ASCII de contenu ;
- sémantique des valeurs ;
- comportement hors plage ;
- lectures pendant transition ;
- robustesse aux défauts de communication.

---

## 4. Références

- Plan de test Modbus TR2 — famille **FT-ACC**
- Référentiel unique de mapping Modbus
- Charte d’arborescence et pipeline de validation hérités de FT-STR

---

## 5. Règles de conformité

Une zone exposée est conforme au titre de FT-ACC-01 si :

- elle est déclarée lisible dans le mapping ;
- une requête de lecture nominale sur cette zone ne renvoie pas d’exception Modbus ;
- la réponse contient exactement le nombre de registres attendu ;
- la lecture seule ne provoque pas de modification observable de l’image registre ;
- le comportement observé est cohérent avec l’attribut d’accès documentaire.

---

## 6. Préconditions

- mapping unifié disponible et gelé pour le périmètre testé ;
- zone ciblée identifiée comme **lisible** dans le référentiel ;
- simulateur disponible en mode nominal déterministe ;
- état simulé stabilisé avant lecture ;
- aucune transition métier non maîtrisée pendant le test ;
- journal d’échanges activé.

---

## 7. Résultats attendus

- toute zone documentée lisible est effectivement lisible ;
- aucune lecture nominale autorisée n’échoue sans justification normative ;
- la longueur retournée est conforme à la plage ciblée ;
- aucune lecture seule ne provoque de modification observable non spécifiée ;
- toute divergence mapping ↔ comportement est tracée comme anomalie ou ambiguïté.

---

## 8. Risques couverts

| Risque | Impact |
|---|---|
| Zone pourtant lisible non accessible | Centrale inutilisable |
| Réponse tronquée ou longueur erronée | Décodage impossible |
| Divergence mapping ↔ comportement réel | Validation compromise |
| Lecture ayant un effet de bord | État implicite non maîtrisé |

---

## 9. Cas de figure

### Nominal

- lecture unitaire d’un registre lisible ;
- lecture complète d’un champ logique ;
- lecture d’une plage bloc complète.

### Limites

- premier registre d’une plage lisible ;
- dernier registre d’une plage lisible ;
- champ multi-registres complet.

### Erreurs

- non couvertes ici ; voir FT-ACC-06 et FT-RBT.

### Robustesse légère

- lectures répétées à état constant ;
- vérification d’innocuité après lecture.

---

## 10. Points critiques

### Distinction accès / sémantique

FT-ACC-01 valide l’accessibilité, pas la vérité métier de la valeur retournée.

### Zones RW lues en lecture seule

Les zones RW sont incluses si elles sont lisibles ; la présente famille ne valide pas ici l’écriture.

### Innocuité

Une lecture ne doit pas modifier l’image registre exposée.

---

## 11. Ambiguïtés à lever si rencontrées

- garantie éventuelle d’atomicité logique lors d’une lecture multi-registres ;
- comportement d’une lecture pendant mise à jour interne ;
- limites de taille de lecture acceptées par implémentation si non documentées.

---

## 12. Critères de réussite

- 100% des zones instanciées déclarées lisibles sont lues sans erreur ;
- 100% des longueurs observées sont conformes au mapping ;
- aucune lecture seule ne modifie les champs observés ;
- toute divergence documentaire est tracée.

---

## 13. Critères d’échec

- exception Modbus sur zone documentée lisible ;
- longueur de réponse incohérente ;
- lecture partielle d’un champ logique complet ;
- effet de bord observable après lecture ;
- incompatibilité mapping ↔ comportement réel.

---

## 14. Anomalies

| Type | Exemple |
|---|---|
| BLOQUANTE | zone lisible inaccessible |
| MAJEURE | longueur de réponse erronée |
| MAJEURE | champ multi-registres incomplet |
| MAJEURE | lecture avec effet de bord |
| SPEC | ambiguïté de permission ou de portée de lecture |

---

## 15. Dépendances

### Amont

- FT-STR validée

### Aval

- FT-ACC-02 à FT-ACC-07
- FT-BLK

---

## 16. Ordre d’exécution

1. Identifier les zones lisibles dans le mapping.
2. Instancier les lectures unitaires par champ logique.
3. Instancier les lectures de champs multi-registres.
4. Instancier les lectures de plages bloc.
5. Vérifier répétabilité et innocuité sur banc.

---

## 17. Livrables

- cas de test génériques `detaille/`
- cas de test instanciés `instancie/`
- index CSV de traçabilité
- vue d’ensemble d’instanciation

---

## 18. Stratégie d’instanciation retenue

### Option C — Hybride

- **1 fiche par champ logique** pour les lectures unitaires / lectures de champ complet ;
- **1 fiche par plage bloc** pour les lectures multi-registres représentatives.

Cette règle s’appuie sur le mapping unifié logique et sur la couverture bloc.

---

## 19. Conclusion

FT-ACC-01 établit la base de lisibilité effective de l’interface Modbus TR2.

Aucune validation d’écriture, de refus d’écriture, de hors-plage ou de robustesse de communication n’entre dans le périmètre de cette sous-famille.
