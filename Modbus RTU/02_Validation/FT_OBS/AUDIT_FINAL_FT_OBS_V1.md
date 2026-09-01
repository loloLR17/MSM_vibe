# FT-OBS — Audit croisé final V1

## 1. Périmètre

Audit final des six sous-familles FT-OBS :
- FT-OBS-01 — États et discriminabilité système ;
- FT-OBS-02 — Validité, disponibilité et fraîcheur ;
- FT-OBS-03 — Alarmes, défauts et historique observable ;
- FT-OBS-04 — Temps, codes et interprétation déterministe ;
- FT-OBS-05 — Campagnes, commandes et corrélation opératoire ;
- FT-OBS-06 — Diagnostic distant et exploitabilité V1.

## 2. Contrôle de branche

Branche de travail `audit/ft-obs-v1`, créée depuis `main` au commit `f1cdf55df81b4ed6de274c7a0568e706dff7b152`.

Au moment de la passe finale : branche 25 commits en avance, 0 en retard, merge-base identique au commit de départ. Les 24 fichiers de sous-familles sont des ajouts sous `FT_OBS`; aucune spécification V1 ni famille gelée antérieure n'a été modifiée.

## 3. Contrôle des frontières

Aucun déplacement d'ownership n'est retenu :
- FT-STR : structure, encodage, snapshots ;
- FT-ACC : accès RO/RW/exceptions ;
- FT-LIM : domaines, réservés, valeurs légales ;
- FT-BLK : règles et transitions intra-bloc ;
- FT-INT : cohérences inter-blocs ;
- FT-CMD : moteur de commandes B5 ;
- FT-SEQ : scénarios fonctionnels complets ;
- FT-RBT : pertes/répétitions/trames hostiles ;
- FT-PER : persistance/reboot/power-cycle ;
- FT-OBS : capacité de la centrale à distinguer/interpréter normativement ce qui est exposé.

## 4. Passe anti-fabrication

Les six sous-familles respectent les invariants suivants :
- compléments métier informatifs exclus de l'oracle ;
- aucune sentinelle globale inventée ;
- aucune signification attribuée aux réservés ;
- aucune égalité inter-blocs inventée ;
- aucune persistance déduite du caractère RO ;
- aucune fraîcheur déduite de la seule présence d'une valeur ou d'un timestamp ;
- aucune fusion arbitraire des états B1..B7 en un état protocolaire synthétique.

## 5. Recouvrements internes FT-OBS

Les recouvrements observés sont des reprises de synthèse et non des doubles ownerships :
- FT-OBS-06 réutilise les verdicts B2/B3/B6 déjà testés en FT-OBS-01/02/05 ;
- FT-OBS-04 consolide l'interprétation des codes/sentinelles sans reprendre les tests de domaine FT-LIM ;
- FT-OBS-03 et FT-OBS-06 partagent B7 mais le premier traite actif/historique, le second l'exploitabilité distante globale.

Aucune contradiction normative n'est retenue.

## 6. Point méthodologique CONDITIONAL

FT-OBS-02 classe la propriété normative `LAST_VALUE_HELD` comme `COVERED`, tandis que l'exécution physique de `TT-OBS-B03-002` est `CONDITIONAL` si le banc ne permet pas de provoquer l'indisponibilité temporaire nécessaire. Cette distinction est cohérente : couverture normative et faisabilité d'instanciation du scénario sont deux dimensions différentes. Aucun reclassement de la propriété n'est requis.

## 7. Verdict système

La V1 fournit suffisamment de discriminants pour une supervision distante déterministe de premier niveau : identité/capacités, qualification du temps, validité/fraîcheur vibratoire, états acquisition/configuration/commande/campagne, santé/diagnostic et historique minimal.

La V1 ne définit pas de verdict synthétique global unique « exploitable/inexploitable ». Une centrale peut construire une politique applicative, mais celle-ci doit rester explicitement hors oracle protocolaire.

## 8. Décision proposée

**FT-OBS V1 est cohérente, auditable et gelable.**

Le gel et le merge dans `main` restent conditionnés au GO final explicite.