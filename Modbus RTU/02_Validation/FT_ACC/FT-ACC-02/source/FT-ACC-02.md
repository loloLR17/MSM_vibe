# FT-ACC-02 — Fiche de spécification

## Écriture des zones RW

---

## 1. Identification

- **ID** : FT-ACC-02
- **Nom** : Écriture des zones RW
- **Famille parente** : FT-ACC
- **Criticité** : P0

---

## 2. Objectif

Valider que toute zone déclarée **modifiable (`RW`)** par le mapping unifié :
- accepte les écritures Modbus conformes ;
- applique techniquement la valeur écrite ;
- restitue cette valeur en lecture immédiate après écriture.

Cette sous-famille valide l’**accessibilité en écriture** et la cohérence **write → read**, sans traiter la validité métier des valeurs.

---

## 3. Périmètre

### Inclus
- écriture unitaire sur registre `RW` ;
- écriture de début, milieu et fin de plage `RW` ;
- écriture multi-registres ;
- écriture de champs `uint32` ;
- écriture de champs `ASCII fixe` ;
- écriture partielle d’une zone `RW` ;
- répétabilité d’écriture ;
- cohérence write → read ;
- cohérence mapping ↔ comportement observé.

### Exclus
- validité métier de la valeur écrite ;
- transitions fonctionnelles ;
- persistance après reboot ;
- comportement en communication dégradée ;
- effets système complexes pilotés par commande.

---

## 4. Références d’entrée

- Mapping unifié logique TR2
- Plan de test Modbus TR2
- FT-STR validée
- FT-ACC-01 validée

---

## 5. Règles de conformité

Une zone `RW` est conforme si :
- l’écriture est acceptée sans exception Modbus ;
- la longueur écrite est conforme ;
- la valeur écrite est restituée en lecture immédiate ;
- le comportement observé reste cohérent avec l’attribut `RW` du mapping.

---

## 6. Préconditions

- FT-STR validée
- FT-ACC-01 validée
- mapping stabilisé
- accès Modbus opérationnel
- simulateur en état stable
- valeur initiale connue ou lisible

---

## 7. Résultats attendus

- chaque zone `RW` est effectivement modifiable ;
- aucune écriture nominale autorisée n’échoue sans justification normative ;
- la relecture immédiate reflète la valeur écrite ;
- aucune divergence mapping ↔ comportement n’est observée.

---

## 8. Risques couverts

| Risque | Impact |
|---|---|
| Zone `RW` non modifiable | protocole inutilisable |
| Écriture acceptée mais ignorée | comportement trompeur |
| Write → read incohérent | perte de confiance centrale |
| Divergence mapping ↔ réel | défaut documentaire ou implémentatif |

---

## 9. Axes de couverture

- écriture unitaire nominale ;
- écriture de frontière de plage `RW` ;
- écriture multi-registres ;
- écriture de champs structurés ;
- écritures partielles ;
- répétabilité ;
- cohérence write → read ;
- conformité mapping ↔ permissions observées.

---

## 10. Critères d’acceptation

La sous-famille est satisfaite si, pour toutes les instanciations retenues :
- chaque cible `RW` est modifiable ;
- la relecture immédiate restitue la valeur écrite ;
- aucune écriture autorisée n’est refusée sans justification ;
- les divergences éventuelles sont tracées comme anomalies ou ambiguïtés de spécification.

---

## 11. Automatisation

**Oui, fortement automatisable.**

Cette sous-famille est compatible avec une génération automatique à partir du mapping :
- choix de la cible ;
- lecture valeur initiale ;
- écriture valeur de test ;
- relecture ;
- assertion.

---

## 12. Conclusion

FT-ACC-02 valide la capacité effective d’écriture technique du protocole TR2 sur toutes les zones déclarées `RW`.  
Elle constitue un prérequis direct avant les familles de refus d’écriture, de limites métier et de robustesse.
