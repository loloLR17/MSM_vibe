# FT-PER-03 — Configuration et état préparé / actif

## 1. Objet

FT-PER-03 examine exclusivement ce que la V1 permet d'affirmer après une frontière réelle de redémarrage concernant :

- la configuration préparée ;
- la configuration active ;
- les identifiants et CRC associés ;
- `config_state` et `config_error_code` ;
- `config_revision_counter` ;
- l'image active 4E.

## 2. Doctrine

Le Bloc 4 définit précisément la logique `préparé / validé / actif`, ses transitions et les effets d'une application réussie. FT-CMD-05 possède l'acceptation/refus de `APPLY CONFIG`, et FT-INT-02 possède l'effet inter-blocs de l'application réussie.

Aucun de ces textes ne définit, en V1, une politique de conservation ou de réinitialisation de ces éléments à travers reboot.

En conséquence, FT-PER-03 ne transforme jamais :

- `ACTIF` en « persistant » ;
- une image 4E RO en « non volatile » ;
- un compteur de révision en « conservé après reset » ;
- une zone préparée RW en « volatile ».

## 3. Conclusion V1

Pour les états et données de configuration examinés ici, la propriété post-reboot est majoritairement **`NOT_DEFINED`**.

Il n'existe donc pas de test V1 PASS/FAIL imposant après RESET SOFTWARE :

- conservation de la configuration active ;
- effacement de la configuration active ;
- conservation de la configuration préparée ;
- effacement de la configuration préparée ;
- maintien d'un état `ACTIF`, `VALIDE`, `BROUILLON` ou retour à `VIDE` ;
- conservation des IDs, CRC ou du compteur de révision.

## 4. Relation avec les familles gelées

- **FT-BLK-04** : structure et logique interne du Bloc 4 ;
- **FT-CMD-05** : acceptation/refus de `APPLY CONFIG` ;
- **FT-INT-02** : effets d'une application réussie sur l'image active et `config_state` ;
- **FT-PER-03** : comportement de ces éléments après reboot, seulement si normativement défini.

FT-INT-02 exclut explicitement la persistance après reboot ou coupure et la délègue à FT-PER.

## 5. Test de caractérisation

Un scénario de reboot avec relevés avant/après peut être exécuté, mais il est **`TRACE_ONLY`** tant qu'aucune règle V1 supplémentaire n'est arbitrée.

- `TT-PER-B04-001` — caractérisation de l'état Bloc 4 avant/après RESET SOFTWARE.

Ce test ne produit aucun FAIL sur la seule différence ou conservation des valeurs observées.

## 6. Dette normative

La persistance de la configuration active et de la configuration préparée reste une candidate prioritaire V1.1 déjà enregistrée dans `EVOLUTIONS_CANDIDATES_V1_1.md`.

## 7. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.
