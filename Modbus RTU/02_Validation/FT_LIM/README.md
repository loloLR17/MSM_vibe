# FT-LIM — Validation des limites et domaines fonctionnels V1

## 1. Objet

La famille **FT-LIM** valide les domaines de valeurs, combinaisons fonctionnelles, états et invariants explicitement définis par la spécification Modbus RTU V1.

Elle intervient après les contrôles structurels et d'accès :
- **FT-STR** : existence, longueur, type, segmentation et cohérence structurelle ;
- **FT-ACC** : droits RW/RO/réservé et atomicité des accès invalides ;
- **FT-LIM** : validité fonctionnelle d'une valeur ou d'une combinaison sur un accès autorisé.

Une valeur métier invalide écrite dans un registre RW valide n'est donc pas automatiquement un accès Modbus invalide.

## 2. Hiérarchie documentaire

```text
Spécification Modbus RTU V1
        ↓
mapping_unifie dérivé
        ↓
FT-LIM source
        ↓
FT-LIM detaille / matrices
        ↓
FT-LIM instancie / index / overview
```

En cas de divergence, la spécification V1 fait foi. Une contradiction avec une source normative supérieure doit être remontée avant modification d'un artefact gelé.

## 3. Doctrine de non-invention

FT-LIM ne crée aucune borne, enum, sentinelle, valeur réservée, masque, unité, relation entre champs, temporisation ou règle de rejet absente du référentiel normatif.

Les exigences insuffisamment définies sont classées selon le cas :
- `NOT_DEFINED` / **NON DÉFINI — À ARBITRER** ;
- `TRACE_ONLY` lorsque la règle existe mais ne fournit pas d'oracle PASS/FAIL suffisamment précis ;
- `CONDITIONAL` lorsque l'oracle est défini mais nécessite un état ou un environnement particulier pour être exercé.

## 4. Organisation V1

| Sous-famille | Périmètre principal |
|---|---|
| FT-LIM-01 | Domaines unitaires de la configuration préparée — Bloc 4 |
| FT-LIM-02 | Contraintes croisées et dynamiques de configuration — Bloc 4 |
| FT-LIM-03 | Validation, CRC et application de configuration — Blocs 4/5 |
| FT-LIM-04 | Commandes, soumission et préconditions fonctionnelles — Bloc 5 |
| FT-LIM-05 | Sélection dynamique de campagne — Bloc 6 |
| FT-LIM-06 | Domaines et cohérence des métadonnées de campagne — Bloc 6 |
| FT-LIM-07 | Domaines et invariants du diagnostic — Bloc 7 |
| FT-LIM-08 | Identification et état système — Blocs 0/1 |
| FT-LIM-09 | Temps et synchronisation — Bloc 2 |
| FT-LIM-10 | Supervision vibratoire — Bloc 3 |

Aucune sous-famille **FT-LIM-11** n'est créée en V1 : l'audit global n'a identifié aucun domaine fonctionnel autonome restant à couvrir. La fermeture est portée par la matrice globale de couverture à la racine de FT_LIM.

## 5. Principes d'instanciation

- Les tests de frontières sont pilotés par la sémantique normative, pas par un nombre mécanique de valeurs autour de chaque champ.
- Les enums couvrent les valeurs normatives utiles et des valeurs réservées pertinentes.
- Les bitfields couvrent les bits définis et les bits réservés lorsque leur valeur est normée.
- Les champs sans domaine fonctionnel explicite ne reçoivent aucune limite inventée à partir de leur nom ou de leur type de représentation.
- Les registres RO ne sont jamais forcés pour provoquer artificiellement un état.
- Les scénarios destructifs ou dépendants du matériel sont marqués conditionnels lorsque l'environnement sûr n'est pas garanti.

## 6. Configuration préparée et application

Pour le Bloc 4, une écriture fonctionnellement hors domaine sur un champ RW préparé peut être acceptée au niveau Modbus tout en rendant la configuration non validable. FT-LIM distingue donc :
1. acceptation de l'écriture sur l'accès RW valide ;
2. état de la configuration préparée ;
3. validation fonctionnelle ;
4. interdiction d'application si invalide ;
5. protection de la configuration active.

Les codes ou comportements détaillés ne sont exigés que lorsqu'ils sont explicitement définis par la V1.

## 7. Commandes

Les commandes du Bloc 5 sont prises en compte sur front montant de `submit`. Le firmware peut remettre `submit` à zéro après considération. Une vérification naïve `write -> read == written` n'est donc pas un oracle FT-LIM approprié pour ce champ.

Les bits réservés d'un bitfield RW restent une contrainte fonctionnelle de valeur lorsque l'adresse et l'opération sont autorisées.

## 8. État de la famille

La famille V1 est structurée en FT-LIM-01 à FT-LIM-10. La grande passe croisée de fermeture a identifié et corrigé les écarts documentaires résiduels sans nécessiter de nouvelle sous-famille métier.

Le verdict final de gel est établi après contrôle de la matrice globale de couverture et du diff complet de la branche d'audit contre `main`.
