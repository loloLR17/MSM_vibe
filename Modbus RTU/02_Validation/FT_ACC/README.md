# FT_ACC — Validation des accès et permissions Modbus

## 1. Objet

La famille **FT_ACC** couvre la validation complète des règles d’accès au mapping Modbus du système TR2.

Elle garantit que :

* les droits d’accès (RO / RW / reserved) sont respectés ;
* les comportements sont déterministes ;
* aucune écriture illégitime n’est possible ;
* le mapping constitue une **source de vérité fiable**.

---

## 2. Position dans le plan de test

FT_ACC est une famille **P0 critique**, exécutée après :

* FT_STR — Conformité structurelle

Elle constitue le socle de validation des interactions Modbus avant :

* les tests fonctionnels métier
* les tests de robustesse avancés

---

## 3. Périmètre couvert

FT_ACC couvre :

* lecture des zones exposées
* écriture des zones RW
* refus d’écriture sur zones RO
* comportement des champs reserved*
* absence d’effets de bord
* accès hors plage ou invalides
* conformité globale mapping ↔ comportement

---

## 4. Décomposition

La famille est structurée en 7 sous-familles :

| ID        | Nom                | Objectif                                              |
| --------- | ------------------ | ----------------------------------------------------- |
| FT-ACC-01 | Lecture            | Vérifier que toutes les zones exposées sont lisibles  |
| FT-ACC-02 | Écriture RW        | Vérifier que les champs RW sont modifiables           |
| FT-ACC-03 | Refus RO           | Vérifier que les champs RO sont protégés              |
| FT-ACC-04 | Reserved           | Vérifier le comportement des champs reserved*         |
| FT-ACC-05 | Effets de bord     | Garantir qu’aucune écriture n’impacte d’autres champs |
| FT-ACC-06 | Accès invalides    | Vérifier les accès hors plage / partiels              |
| FT-ACC-07 | Conformité globale | Vérifier la cohérence mapping ↔ comportement          |

---

## 5. Doctrine de gouvernance

### 5.1 Accès invalides (règle critique)

Toute requête Modbus invalide doit :

* générer une **exception Modbus explicite**
* ne produire **aucune modification mémoire**
* ne jamais être exécutée partiellement

Aucun comportement implicite n’est autorisé.

---

### 5.2 Champs reserved*

Les champs nommés `reserved*` doivent respecter :

* lecture : autorisée si exposée
* écriture :

  * refusée, ou
  * acceptée sans effet observable
* comportement :

  * neutre
  * stable
  * sans impact système

---

### 5.3 Mapping = source de vérité

Le mapping unifié :

* définit le comportement attendu
* doit être strictement respecté par le firmware
* ne doit jamais être contredit par l’implémentation

---

## 6. Méthodologie de test

Chaque sous-famille suit la structure standard :

```
FT-ACC-0X/
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

### Niveaux de test

* **GEN** : cas génériques (logiques)
* **instancié** : cas réels basés sur le mapping

---

## 7. Règles d’industrialisation

* 1 test instancié par champ ou par scénario réel
* index CSV systématique
* overview synthétique obligatoire
* traçabilité complète test ↔ mapping
* nomenclature normalisée :

```
TT-ACC-XX-B<bloc>-XXX
```

---

## 8. Critères de validation globale

La famille FT_ACC est validée si :

* aucun champ RO n’est modifiable
* tous les champs RW sont modifiables
* les champs reserved restent neutres
* aucune écriture ne génère d’effet de bord
* tous les accès invalides sont refusés correctement
* le comportement réel est conforme au mapping à 100 %

---

## 9. Automatisation

FT_ACC est :

* fortement automatisable
* compatible avec un banc de test Modbus
* structuré pour exécution batch complète

---

## 10. Résultat attendu

À l’issue de FT_ACC :

* le protocole Modbus est **fiable**
* le mapping est **exploitable sans ambiguïté**
* la centrale peut s’appuyer dessus en confiance

---

## 11. Conclusion

FT_ACC constitue :

👉 le **socle de sécurité et de cohérence du protocole**

C’est une famille indispensable pour :

* éviter les comportements implicites
* garantir la robustesse terrain
* permettre l’industrialisation du système
