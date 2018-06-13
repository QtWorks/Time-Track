/* jshint indent: 1 */

module.exports = function(sequelize, DataTypes) {
  return sequelize.define(
    'task',
    {
      id: {
        type: DataTypes.INTEGER(11),
        allowNull: false,
        primaryKey: true,        
        autoIncrement: true
      },
      name: {
        type: DataTypes.STRING(45),
        allowNull: false,
      },
      numeric: {
        type: DataTypes.INTEGER(4),
        allowNull: false
      },
      is_active: {
        type: DataTypes.INTEGER(4),
        allowNull: false,
      },
      subcategory_id: {
        type: DataTypes.INTEGER(11),
        allowNull: false,
        references: {
          model: 'subcategory',
          key: 'id',
        },
      },
    },
    {
      tableName: 'task',
      timestamps: false,
    },
  );
};
