# FT-ACC-02 — Écriture autorisée des zones RW

## 1. Identification

- **ID** : FT-ACC-02
- **Nom** : Écriture autorisée des zones RW
- **Famille parente** : FT-ACC — Accès et permissions
- **Criticité** : P0

## 2. Objectif

Valider qu'un registre ou champ explicitement déclaré `RW` par la spécification V1 et le mapping dérivé accepte une écriture Modbus **autorisée et correctement bornée**.

FT-ACC-02 vérifie le droit d'écriture technique. Elle ne transforme pas l'attribut `RW` en autorisation implicite d'écrire n'importe quelle sous-partie d'un champ logique ni de traverser des registres non inscriptibles.

## 3. Référentiel

Ordre de vérité :

1. spécification Modbus RTU V1 gelée ;
2. GEL-MAP-V1, mapping dérivé ;
3. présente fiche source ;
4. tests génériques ;
5. tests instanciés.

Pré-requis : FT-STR gelée, notamment FT-STR-06 pour l'exposition structurelle des adresses.

## 4. Périmètre inclus

- écriture nominale d'un registre mono-registre `RW` ;
- écriture nominale d'un champ logique multi-registres entièrement `RW` ;
- écriture d'une plage contiguë uniquement lorsqu'**chaque registre** de la plage est RW et que la V1 n'impose pas une granularité plus forte ;
- utilisation de FC06 pour un registre unique lorsque la V1 et l'implémentation l'autorisent ;
- utilisation de FC16 pour plusieurs registres contigus RW ;
- relecture de contrôle lorsque la sémantique V1 permet d'attendre une image stable de la valeur écrite ;
- cohérence entre droits `RW` V1, mapping dérivé et comportement observé.

## 5. Hors périmètre

- écriture d'un registre `RO`, réservé ou non exposé ;
- requêtes couvrant à la fois des registres RW et non-RW ;
- écritures partielles arbitraires à l'intérieur d'un `uint32` ou d'un champ `ASCII fixe` en l'absence d'une règle V1 explicite ;
- validité métier d'une valeur écrite dans un champ RW ;
- validation, application ou rejet fonctionnel d'une configuration ;
- effets secondaires fonctionnels explicitement définis par V1 ;
- persistance après redémarrage ;
- robustesse sur trames invalides ou communication dégradée.

Les écritures interdites relèvent de FT-ACC-03, FT-ACC-04 et FT-ACC-06. Les valeurs métier relèvent de FT-LIM ou de la famille fonctionnelle concernée. Les effets secondaires relèvent de FT-ACC-05 lorsqu'ils concernent l'intégrité des accès.

## 6. Règles de conformité

### 6.1 Cible mono-registre RW

Une écriture nominale est conforme si :

- la cible est explicitement RW ;
- la fonction Modbus utilisée est autorisée ;
- aucune exception Modbus n'est retournée ;
- la valeur est effectivement prise en compte au niveau de l'image préparée ou du registre concerné, sous réserve de la sémantique V1 ;
- aucun registre non ciblé n'est modifié en dehors d'un effet explicitement normatif.

### 6.2 Champ logique multi-registres RW

Pour `uint32` et `ASCII fixe`, le scénario nominal porte sur le **champ complet**.

Une écriture partielle n'est pas considérée comme exigence de conformité FT-ACC-02 sauf si la V1 l'autorise explicitement pour le champ concerné.

### 6.3 Plage contiguë

Une écriture FC16 de plage n'est nominalement valide que si tous les registres couverts par la requête sont inscriptibles.

Une plage qui contient au moins un registre RO, réservé ou non exposé n'est pas une "plage RW" ; son traitement appartient aux accès invalides et doit respecter GEL-GOV-02.

### 6.4 Valeurs métier

Une valeur hors domaine écrite dans une cible explicitement RW ne constitue pas à elle seule un accès Modbus invalide.

FT-ACC-02 ne conclut donc pas sur la validité métier de cette valeur. Les jeux de test nominaux utilisent de préférence des valeurs V1 connues comme sûres afin d'éviter de confondre droit d'accès et logique fonctionnelle.

### 6.5 Relecture

La relecture immédiate doit confirmer la prise en compte lorsque le registre représente une donnée mémorisée ou préparée stable.

Pour un registre de commande, déclencheur ou champ dont V1 définit une évolution autonome après écriture, le critère de conformité porte sur l'acceptation de l'accès et sur le comportement explicitement défini par V1 ; l'égalité brute `valeur relue = valeur écrite` n'est pas imposée sans base normative.

## 7. Axes de couverture génériques

1. registre RW mono-registre ;
2. champ `uint32` RW complet ;
3. champ `ASCII fixe` RW complet ;
4. plage contiguë entièrement RW avec FC16 ;
5. répétition d'une écriture nominale autorisée ;
6. cohérence permission V1 ↔ mapping ↔ comportement.

## 8. Critères d'acceptation

FT-ACC-02 est satisfaite si :

- chaque cible RW retenue est accessible en écriture selon une granularité explicitement défendable par V1 ;
- aucune cible nominale entièrement RW n'est refusée sans justification normative ;
- aucune plage mélangeant RW et non-RW n'est présentée comme écriture nominale ;
- les tests ne déduisent aucune règle d'écriture partielle absente de V1 ;
- les verdicts distinguent clairement permission Modbus, validité métier et comportement fonctionnel.

## 9. Automatisation

Fortement automatisable à partir de GEL-MAP-V1, sous réserve d'une sélection des valeurs de test sûre et d'un traitement spécifique des registres à sémantique de commande.

## 10. Conclusion

FT-ACC-02 valide strictement le **droit d'écriture autorisé** des éléments RW de l'interface Modbus TR2, sans empiéter sur FT-LIM, les fonctions métier ou les tests d'accès interdits.
