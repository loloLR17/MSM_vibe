# FT-INT — Cohérence inter-blocs

## 1. Objet

FT-INT valide les relations fonctionnelles normatives dont l'oracle nécessite l'observation d'au moins deux blocs Modbus distincts.

Une relation n'appartient à FT-INT que si la spécification V1 établit explicitement un lien fonctionnel inter-blocs permettant de construire un oracle. Le simple fait qu'un scénario manipule plusieurs blocs ne suffit pas.

## 2. Frontières

FT-INT ne revalide pas :
- la structure et l'encodage : FT-STR ;
- les droits d'accès : FT-ACC ;
- les domaines et limites : FT-LIM ;
- les invariants internes à un bloc : FT-BLK ;
- le moteur transactionnel du Bloc 5 : FT-CMD ;
- les scénarios complets multi-actions : FT-SEQ ;
- la robustesse et les timings hostiles : FT-RBT ;
- la persistance, le reboot et les coupures : FT-PER.

FT-INT peut utiliser une commande B5 réussie comme stimulus afin d'en vérifier l'effet observable dans un autre bloc, sans requalifier l'acceptation, l'idempotence, les codes résultat ni le moteur de commande lui-même.

## 3. Classification

- `COVERED` : oracle V1 déterministe et testable ;
- `CONDITIONAL` : exigence normative réelle mais nécessitant un contexte, un banc ou une injection contrôlée ;
- `DELEGATED` : exigence réelle dont la validation appartient à une autre famille ;
- `TRACE_ONLY` : relation utile à la traçabilité mais insuffisamment définie pour un verdict PASS/FAIL ;
- `NOT_DEFINED` : relation plausible ou attendue mais sans oracle normatif V1 exploitable.

## 4. Convention d'identification

Les cas détaillés utilisent la forme `TT-INT-<scope>-<numéro>`, où `<scope>` identifie les blocs effectivement observés, par exemple `TT-INT-B02B05-001`.

## 5. Sous-familles prévues

- `FT-INT-01_Temps_commandes` : relations B2 ↔ B5 liées à la synchronisation temporelle ;
- `FT-INT-02_Configuration_application` : relations B4 ↔ B5 liées à l'application de configuration ;
- `FT-INT-03_Configuration_supervision` : relations B4 ↔ B3 liées aux paramètres actifs de supervision ;
- `FT-INT-04_Acquisition_campagnes` : relations B5 ↔ B1 ↔ B6 et bases temporelles de campagne ;
- `FT-INT-05_Etat_diagnostic_transversal` : relations transversales d'état et de diagnostic.

## 6. Statut

Famille en reconstruction sur la branche d'audit FT-INT. Aucun gel V1 avant audit croisé complet et validation explicite.
