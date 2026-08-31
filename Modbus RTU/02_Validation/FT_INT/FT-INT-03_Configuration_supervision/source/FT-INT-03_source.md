# FT-INT-03 — Source normative B4↔B3

## 1. Références V1

- Bloc 3 — Supervision vibratoire : les seuils appliqués aux indicateurs sont ceux de la configuration active au moment du calcul ; un changement de configuration ne rétroagit pas sur les valeurs déjà calculées et prend effet à partir de la prochaine fenêtre de calcul validée.
- Bloc 4 — Configuration acquisition : les seuils de supervision sont préparés dans B4 et ne deviennent actifs qu'après activation de la configuration.
- FT-BLK-03 : la décision de seuil B3 à partir de la configuration active B4 est explicitement déléguée à FT-INT.
- FT-INT-02 / FT-CMD : l'application de la configuration B4 via B5 n'est pas revalidée ici.

## 2. Règles de couverture

### INT03-R01 — Utilisation de la configuration active

**Statut : CONDITIONAL**

Le Bloc 3 doit évaluer ses résultats de supervision à partir des seuils appartenant à la configuration B4 active au moment du calcul.

Oracle : avec une stimulation déterministe, le verdict B3 doit correspondre aux seuils actifs, indépendamment d'une autre configuration seulement préparée.

### INT03-R02 — Absence d'effet de la configuration seulement préparée

**Statut : CONDITIONAL**

La préparation ou modification de seuils B4 sans activation ne doit pas modifier la décision de supervision produite par une nouvelle fenêtre B3.

Oracle : tant que l'ancienne configuration reste active, un même stimulus déterministe doit conserver le verdict correspondant à cette configuration active.

### INT03-R03 — Absence de rétroactivité

**Statut : COVERED**

L'activation d'une nouvelle configuration ne doit pas réinterpréter ni recalculer rétroactivement les valeurs B3 déjà issues d'une fenêtre validée.

Oracle minimal : entre l'activation et la prochaine fenêtre validée, le snapshot déjà validé ne doit pas être transformé uniquement du fait du changement de configuration.

Cette règle ne fixe aucune latence arbitraire et ne suppose pas qu'un snapshot doive rester exposé si une nouvelle fenêtre valide survient immédiatement.

### INT03-R04 — Prise d'effet à la prochaine fenêtre validée

**Statut : CONDITIONAL**

Après activation d'une nouvelle configuration B4, les nouveaux seuils doivent être utilisés à partir de la prochaine fenêtre de calcul B3 validée.

Oracle : la première fenêtre validée après activation doit produire une décision compatible avec les nouveaux seuils actifs.

### INT03-R05 — Unité commune mg

**Statut : TRACE_ONLY / DELEGATED**

Les grandeurs vibratoires B3 et les seuils B4 sont exprimés en mg en V1. Cette cohérence rend la relation directement interprétable sans conversion.

La validation des domaines, types et unités appartient à FT-LIM / FT-STR ; FT-INT-03 n'en recrée pas le test.

### INT03-R06 — Dérivation exhaustive vers les états de supervision

**Statut : NOT_DEFINED**

La V1 ne fournit pas un oracle exhaustif permettant de déduire, pour toute combinaison de seuils et de mesures, l'ensemble de `B3_STATUS_GLOBAL`, `B3_SEVERITY_GLOBAL`, `B3_ALARM_FLAGS` et autres états dérivés.

Aucune table de vérité supplémentaire ne doit être inventée.

### INT03-R07 — Hystérésis et temporisation d'alarme

**Statut : NOT_DEFINED / CONDITIONAL**

Les paramètres `threshold_hysteresis_mg` et `alarm_hold_time_ms` existent dans B4, mais FT-INT-03 ne doit imposer une chronologie précise de franchissement, maintien ou retour que si cette chronologie est explicitement définie par les spécifications V1 applicables.

Toute vérification plus précise nécessite un oracle documentaire explicite et un stimulus temporel déterministe.

### INT03-R08 — B3 CONFIG_VALID versus état B4

**Statut : NOT_DEFINED**

La présence du bit `B3_VALIDITY_FLAGS.CONFIG_VALID` ne suffit pas à établir une égalité normative exacte avec une valeur particulière de `config_state` B4.

Il est interdit d'imposer par déduction une règle du type `CONFIG_VALID == (config_state == ACTIF)` sans texte V1 explicite.

## 3. Doctrine anti-duplication

FT-INT-03 ne reteste pas :
- les calculs RMS/crête internes B3 : FT-BLK-03 ;
- les cohérences internes entre champs B3 : FT-BLK-01/03 ;
- les domaines et valeurs limites des seuils : FT-LIM ;
- la structure, atomicité et snapshot : FT-STR ;
- la mécanique d'application de configuration B5 : FT-CMD et FT-INT-02.

## 4. Doctrine anti-fabrication

Aucun verdict PASS/FAIL ne doit dépendre :
- d'une correspondance exhaustive seuil → sévérité non spécifiée ;
- d'une égalité implicite entre un flag B3 et un état B4 ;
- d'une latence inventée entre activation et fenêtre suivante ;
- d'une chronologie d'hystérésis/maintien absente de la V1.
