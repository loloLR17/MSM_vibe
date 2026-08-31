# FT-STR-02 — Tests instanciés GEL-MAP-V1

## Objet

Contrôler le type déclaré et la taille structurelle des 183 champs logiques du mapping unifié gelé.

## Références

- V1 : `01_Specification_source/bloc0.md` à `bloc7.md`
- charte : `01_Specification_source/charte_typage.md`
- mapping : GEL-MAP-V1
- gel mapping : `ff948e5917becceed7637d9c7864ec9b279be0ca`
- génériques : `TT-STR-02-GEN-001` à `TT-STR-02-GEN-003`

## Règle d'instanciation

Une fiche par champ logique contrôle :

1. le type déclaré dans GEL-MAP-V1 par rapport à V1/charte ;
2. l'appartenance à la liste des types autorisés ;
3. la compatibilité entre le type et le nombre de registres.

Les adresses présentes dans les fiches servent à identifier sans ambiguïté l'instanciation. Leur géométrie est validée par FT-STR-01.

Une lecture Modbus n'est pas utilisée comme preuve du type : `uint16`, `int16`, `enum16` et `bitfield16` transportent tous un mot brut de 16 bits.

## Couverture

- Bloc 0 : 10 champs logiques
- Bloc 1 : 18 champs logiques
- Bloc 2 : 12 champs logiques
- Bloc 3 : 26 champs logiques
- Bloc 4 : 66 champs logiques
- Bloc 5 : 18 champs logiques
- Bloc 6 : 20 champs logiques
- Bloc 7 : 13 champs logiques
- **Total : 183 champs logiques**

## Convention

`TT-STR-02-B<bloc>-NNN`

## Statut documentaire

Les 183 champs sont issus de `tr2_mapping_unifie_logique.csv`. GEN-003 est également applicable globalement à l'ensemble des artefacts actifs FT-STR-02.
