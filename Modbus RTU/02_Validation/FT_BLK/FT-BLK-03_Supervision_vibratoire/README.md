# FT-BLK-03 — Supervision vibratoire et cohérence des résultats

## 1. Objet

FT-BLK-03 valide les règles fonctionnelles internes du Bloc 3 qui disposent d'un oracle normatif V1 : conventions de calcul vibratoire, comportement des métadonnées de calcul, maintien conditionnel de la dernière valeur et compteurs de supervision.

## 2. Doctrine

La sous-famille distingue strictement :
- **COVERED** : oracle V1 directement exécutable ;
- **CONDITIONAL** : oracle V1 existant mais nécessitant un moyen de stimulation déterministe ou une condition d'implémentation ;
- **NOT_DEFINED** : la V1 ne définit pas suffisamment la fonction de dérivation ;
- **DELEGATED** : la relation dépend d'un autre bloc ou d'une autre famille.

Aucun algorithme absent de la V1 n'est inventé.

## 3. Périmètre actif

FT-BLK-03 couvre :
- calcul RMS global sur la norme vectorielle ;
- calcul crête globale sur la norme vectorielle ;
- calcul RMS par axe ;
- calcul crête par axe ;
- comportement conditionnel de dernière valeur conservée ;
- stabilité de `B3_CALC_SEQUENCE` en l'absence de nouvelle fenêtre validée ;
- monotonie des compteurs `B3_EXCEED_COUNT` et `B3_ALARM_COUNT`.

## 4. Limitations V1 conservées explicitement

Ne sont pas transformés en oracles artificiels :
- dérivation de `B3_DOMINANT_AXIS` : critère de dominance non spécifié ;
- dérivation complète `B3_STATUS_GLOBAL` ↔ `B3_VALIDITY_FLAGS` ;
- dérivation `B3_SEVERITY_GLOBAL` ↔ alarmes ;
- formule de validation du nombre d'échantillons / fenêtre ;
- politique exacte de saturation / wrap des compteurs au-delà de l'autorisation de saturation ;
- décision de seuil B3 à partir de la configuration active B4.

## 5. Hors périmètre

- cohérence `B3_EXCEED_*` ↔ bits d'alarme et `B3_ALARM_LATCHED` ↔ bit mémorisé : FT-BLK-01 ;
- monotonie générale de `B3_CALC_SEQUENCE` : FT-BLK-02 ;
- domaines de valeurs : FT-LIM ;
- structure / snapshot / atomicité : FT-STR ;
- seuils actifs B4 → résultats B3 : FT-INT ;
- reset/persistance des compteurs : FT-PER.

## 6. Artefacts actifs

- `source/FT-BLK-03_source.md` ;
- `detaille/FT-BLK-03_detaille.md` ;
- `detaille/FT-BLK-03_matrice_couverture.csv`.

## 7. Statut

Sous-famille reconstruite sur le cadrage validé. Les tests numériques restent conditionnels tant qu'un banc d'injection ou de rejeu déterministe n'existe pas. Gel interdit avant audit croisé et validation explicite.
