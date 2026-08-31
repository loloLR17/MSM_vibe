# Audit final FT-STR — GEL-MAP-V1

## Objet

Ce document clôt l'audit croisé de la famille **FT-STR — Conformité structurelle** après reconstruction des sous-familles FT-STR-01 à FT-STR-08.

## Référentiel

Ordre de vérité appliqué :

1. spécification Modbus RTU V1 gelée (`bloc0.md` à `bloc7.md`, `charte_typage.md`) ;
2. GEL-MAP-V1, artefact dérivé ;
3. sources de validation FT-STR ;
4. tests génériques ;
5. instanciations et résultats.

Gouvernance appliquée : GEL-GOV-01, GEL-GOV-02, GEL-GOV-03 et `CHARTE_ARBORESCENCE.md`.

## Conclusion de l'audit croisé

Les sous-familles suivantes sont cohérentes entre elles et avec la hiérarchie documentaire :

- FT-STR-08 — conformité documentaire ;
- FT-STR-01 — structure des plages ;
- FT-STR-02 — typage des champs ;
- FT-STR-03 — encodage multi-registres ;
- FT-STR-04 — ASCII fixe ;
- FT-STR-05 — registres réservés et sentinelles structurelles ;
- FT-STR-06 — accessibilité et découpage de lecture Modbus ;
- FT-STR-07 — stabilité d'image et cohérence temporelle.

Le pipeline de référence est : **08 → 01 → 02 → 03 → 04 → 05 → 06 → 07**.

## Points gelés à l'issue de l'audit

- V1 reste la source normative supérieure ; GEL-MAP-V1 reste un artefact dérivé.
- Les anciens artefacts pré-renforcement, lorsqu'ils existent, sont isolés dans `archive_pre_renforcement/` et n'ont aucun rôle actif.
- Une sous-plage valide peut être lue sans obligation de couvrir un champ logique complet.
- Les lacunes inter-blocs sont non exposées et ne sont pas des registres réservés.
- `0` n'est pas une sentinelle globale ; toute sémantique spéciale doit être explicitement définie par V1.
- Les réservés doivent être identifiables sans ambiguïté ; leur nom peut rester celui explicitement défini par V1, notamment `B3_RESERVED_0`.
- Les refus d'écriture et droits d'accès relèvent de FT-ACC / GEL-GOV-02, pas de FT-STR.
- Les réponses multi-registres doivent respecter un même instant logique conformément à `charte_typage.md`.
- Plusieurs requêtes successives ne constituent pas, par elles-mêmes, un snapshot atomique unique.
- Aucun test terrain non exécuté n'est présenté comme PASS.

## État documentaire

L'arborescence active FT-STR est cohérente avec `CHARTE_ARBORESCENCE.md` : chaque sous-famille possède une fiche source active et un niveau générique lorsqu'il est applicable ; les instanciations actives sont séparées des archives.

Le README racine de famille est présent et documente la hiérarchie, le pipeline et la doctrine active.

La contradiction documentaire antérieure imposant systématiquement le préfixe `reserved_` a été corrigée dans `Specifications.md` sans modifier V1 ni GEL-MAP-V1.

## Limite du gel

Le présent gel porte sur la **cohérence documentaire, structurelle et méthodologique** de la suite FT-STR.

Il ne remplace pas l'exécution sur firmware/capteur des contrôles qui exigent une cible réelle, notamment :

- comportement FC03 effectif ;
- ordre d'octets ASCII réellement exposé ;
- lecture à zéro des réservés lorsque V1 l'impose ;
- atomicité dynamique des `uint32` ;
- cohérence temporelle intra-réponse sur données évolutives.

Ces points restent à exécuter et à enregistrer dans le cadre de la validation matérielle.

## Statut

**FT-STR — audit documentaire et méthodologique terminé.**

Aucune anomalie structurelle documentaire bloquante connue ne subsiste à l'issue de cette passe croisée.
